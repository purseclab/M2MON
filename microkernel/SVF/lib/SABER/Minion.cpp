//===- Minion.cpp -- Minion analysis pass ------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2017-2018>  <Chung Hwan Kim>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//

/*
 * Minion.cpp
 *
 *  Created on: Jul 1, 2017
 *      Author: Chung Hwan Kim
 */

//#define MINION_DEBUG

#include "SABER/Minion.h"
#include "Util/AnalysisUtil.h"
#include <llvm/Support/CommandLine.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/InstIterator.h>
#include <map>

using namespace llvm;
using namespace analysisUtil;

typedef std::map<NodeID, int> GlobalACL; // Global PAG node -> Unix file mode
typedef std::map<const llvm::Function *, GlobalACL> FunctionACL;

char Minion::ID = 0;

static RegisterPass<Minion> MINION("minion", "Minion Analysis Pass");

static NodeSet globPAGNodes;
static FunctionACL fnACL;

static const char *getSVFGNodeTypeString(const SVFGNode *node) {
    if (isa<StmtSVFGNode>(node))
        return "StmtSVFGNode";
    if (isa<FormalParmSVFGNode>(node))
        return "FormalParmSVFGNode";
    if (isa<ActualParmSVFGNode>(node))
        return "ActualParmSVFGNode";
    if (isa<ActualRetSVFGNode>(node))
        return "ActualRetSVFGNode";
    if (isa<FormalRetSVFGNode>(node))
        return "FormalRetSVFGNode";
    if (isa<FormalINSVFGNode>(node))
        return "FormalINSVFGNode";
    if (isa<FormalOUTSVFGNode>(node))
        return "FormalOUTSVFGNode";
    if (isa<ActualINSVFGNode>(node))
        return "ActualINSVFGNode";
    if (isa<ActualOUTSVFGNode>(node))
        return "ActualOUTSVFGNode";
    if (isa<MSSAPHISVFGNode>(node))
        return "MSSAPHISVFGNode";
    if (isa<NullPtrSVFGNode>(node))
        return "NullPtrSVFGNode";
    return "Unknown";
}

static const char *getACLModeString(int mode) {
    switch (mode) {
        case 0: return "";
        case 1: return "x";
        case 2: return "w";
        case 3: return "wx";
        case 4: return "r";
        case 5: return "rx";
        case 6: return "rw";
        case 7: return "rwx";
    }
    return NULL;
}

static void insertACL(const llvm::Function *fn, NodeID pagNodeId, int mode) {
    if (fnACL.find(fn) == fnACL.end())
        fnACL[fn] = GlobalACL();
    GlobalACL& globACL = fnACL[fn];
    if (globACL.find(pagNodeId) == globACL.end())
        globACL[pagNodeId] = 0;
    globACL[pagNodeId] |= mode;
}

static bool indirectSVFGEdgeInForwardSlice(const SVFGNode *node,
                                           ProgSlice *slice) {
    for (auto i = node->InEdgeBegin(); i != node->InEdgeEnd(); ++i) {
        const SVFGEdge *edge = *i;

        const IndirectSVFGEdge *inEdge = dyn_cast<IndirectSVFGEdge>(edge);
        if (inEdge == NULL)
            continue;

        const SVFGNode* srcNode = inEdge->getSrcNode();
        if (!slice->inForwardSlice(srcNode))
            continue;

        //const PointsTo& inPointsTo = inEdge->getPointsTo();
        return true;
    }

    return false;
}

static int pointerDerefInFunction(const StmtSVFGNode *stmtNode) {
    int ret = 0;

    const llvm::Instruction *operand = stmtNode->getInst();
    if (operand == NULL)
        return ret;

    const llvm::Function *fn = operand->getFunction();
    if (fn == NULL)
        return ret;

    for (auto i = llvm::inst_begin(*fn); i != llvm::inst_end(*fn); ++i) {
        int mod = 0;
        if (isa<llvm::StoreInst>(&*i)) {
            if (i->getOperand(1) != operand)
                continue;
            mod = 2;
        } else if (isa<llvm::LoadInst>(&*i)) {
            if (i->getOperand(0) != operand)
                continue;
            mod = 4;
        } else {
            continue;
        }

#ifdef MINION_DEBUG
        errs() << "Deref:  SVFG=" << stmtNode->getId()
               << " Mode=" << getACLModeString(mod)
               << " Instruction={" << *i << "}"
               << " Operand={" << *operand << "}"
               << " Function=" << fn->getName() << "\n";
#endif

        ret |= mod;
    }

    return ret;
}

/*!
 * Initialize sources
 */
void Minion::initSrcs() {
    PAG *pag = getPAG();
    const SVFG *svfg = getSVFG();

    // Find all global variables
    for (auto &glob : this->mod->getGlobalList()) {
        // Ignore global variables with no symbol
        if (glob.getName().startswith("."))
            continue;

        NodeID pagNodeId = pag->getValueNode(&glob);
        globPAGNodes.insert(pagNodeId);

        const PAGNode *pagNode = pag->getPAGNode(pagNodeId);
        const SVFGNode *node = svfg->getDefSVFGNode(pagNode);

        addToSources(node); // Add as a source to "points-to" analysis

#ifdef MINION_DEBUG
        errs() << "Source: " << glob
               << " (PAG=" << pagNodeId << ", SVFG=" << node->getId() << ")\n";
#endif
    }

    // Find reads from and writes to the global variables
    for (auto i = svfg->begin(); i != svfg->end(); ++i) {
        const SVFGNode *node = i->second;

        const StmtSVFGNode *stmtNode = dyn_cast<StmtSVFGNode>(node);
        if (stmtNode == NULL)
            continue;

        const llvm::BasicBlock *bb = node->getBB();
        if (bb == NULL)
            continue;

        const llvm::Function *fn = bb->getParent();
        if (fn == NULL)
            continue;

        NodeID pagSrcNodeId = stmtNode->getPAGSrcNodeID();
        if (globPAGNodes.find(pagSrcNodeId) != globPAGNodes.end()) {
            insertACL(fn, pagSrcNodeId, 4);

#ifdef MINION_DEBUG
            errs() << "Read:   SVFG=" << node->getId()
                   << " PAG=" << pagSrcNodeId
                   << " Function=" << fn->getName() << "\n";
#endif
        }

        NodeID pagDstNodeId = stmtNode->getPAGDstNodeID();
        if (globPAGNodes.find(pagDstNodeId) != globPAGNodes.end()) {
            insertACL(fn, pagDstNodeId, 2);

#ifdef MINION_DEBUG
            errs() << "Write:  SVFG=" << node->getId()
                   << " PAG=" << pagDstNodeId
                   << " Function=" << fn->getName() << "\n";
#endif
        }
    }
}

/*!
 * Initialize sinks
 */
void Minion::initSnks() {
#if 0
    PAG *pag = getPAG();
    const SVFG *svfg = getSVFG();

    // Add all global variables as sinks to "points-to" analysis
    for (auto i = globPAGNodes.begin(); i != globPAGNodes.end(); ++i) {
        NodeID pagNodeId = *i;

        const PAGNode *pagNode = pag->getPAGNode(pagNodeId);
        const SVFGNode *node = svfg->getDefSVFGNode(pagNode);

        addToSinks(node);

#ifdef MINION_DEBUG
        errs() << "Sink:   " << *pagNode->getValue()
               << " (PAG=" << pagNodeId << ", SVFG=" << node->getId() << ")\n";
#endif
    }
#endif

}

void Minion::reportBug(ProgSlice* slice) {
    const SVFGNode *src = slice->getSource();

    const StmtSVFGNode *srcStmtNode = dyn_cast<StmtSVFGNode>(src);
    if (srcStmtNode == NULL)
        return;

    NodeID srcPAGNodeId = srcStmtNode->getPAGDstNodeID();

#ifdef MINION_DEBUG
    errs() << "Slice:  SVFG=" << srcStmtNode->getId() << "\n";
#endif

    // Check if the source is eventually de-referenced.
    int mod = 0;
    for (auto i = slice->forwardSliceBegin();
        i != slice->forwardSliceEnd(); ++i) {
        const SVFGNode *node = *i;

        const StmtSVFGNode *stmtNode = dyn_cast<StmtSVFGNode>(node);
        if (stmtNode == NULL)
            continue;

        const PAGEdge *pagEdge = stmtNode->getPAGEdge();
        if (!isa<LoadPE>(pagEdge)) {
            continue;
        }

        // Check if there is any indirect SVF edge from another node in the
        // slice to this node.
        if (!indirectSVFGEdgeInForwardSlice(node, slice))
            continue;

        // Find all pointer dereferences of the destination value of this node
        // in the function and retrieve the corresponding access mode.
        mod |= pointerDerefInFunction(stmtNode);
    }

    // No de-reference?
    if (mod == 0)
        return;

    // Add access control rule for every function in the slice.
    for (auto i = slice->forwardSliceBegin();
        i != slice->forwardSliceEnd(); ++i) {
        const SVFGNode *node = *i;

        const StmtSVFGNode *stmtNode = dyn_cast<StmtSVFGNode>(node);
        if (stmtNode == NULL)
            continue;

        const llvm::Instruction *inst = stmtNode->getInst();
        if (inst == NULL)
            continue;

        const llvm::Function *fn = inst->getFunction();
        if (fn == NULL)
            continue;

        insertACL(fn, srcPAGNodeId, mod);

#ifdef MINION_DEBUG
        errs() << "Point:  SVFG=" << node->getId()
               << " PAG=" << srcPAGNodeId
               << " Mode=" << getACLModeString(mod)
               << " Function=" << fn->getName() << "\n";
#endif
    }
}

void Minion::finalize() {
    SrcSnkDDA::finalize();

    PAG *pag = getPAG();

    for (auto i = fnACL.begin(); i != fnACL.end(); ++i) {
        const llvm::Function *fn = i->first;
        GlobalACL& globACL = i->second;

        std::cout << "Function: '" << fn->getName().str() << "'" << endl;

        for (auto j = globACL.begin(); j != globACL.end(); ++j) {
            NodeID pagNodeId = j->first;
            int mode = j->second;

            const PAGNode *pagNode = pag->getPAGNode(pagNodeId);
            if (!pagNode->hasValue())
                continue;
            const llvm::Value *val = pagNode->getValue();

            std::cout << "  " << val->getName().str()
                      << " " << getACLModeString(mode) << endl;
        }

        std::cout << endl;
    }
}
