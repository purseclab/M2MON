//===- CGraph.cpp -- CGraph analysis pass ------------------------------------//
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
 * CGraph.cpp
 *
 *  Created on: Jul 4, 2017
 *      Author: Chung Hwan Kim
 */
#ifdef MINION_CGRAPH

//#define CGRAPH_DEBUG

#include "SABER/CGraph.h"
#include "Util/AnalysisUtil.h"
#include <llvm/Support/CommandLine.h>
#include <map>

using namespace llvm;
using namespace analysisUtil;

struct MyCallGraphObj {
    NodeSet dstNodes;
    unsigned long callers;
    MyCallGraphObj() : callers(0) {}
};

typedef std::map<NodeID, MyCallGraphObj> MyCallGraph;

char CGraph::ID = 0;

static RegisterPass<CGraph> CGRAPH("cgraph", "Call Graph Construction");

static MyCallGraph myGraph;

static void insertCall(NodeID srcNodeId, NodeID dstNodeId) {
    if (myGraph.find(srcNodeId) == myGraph.end())
        myGraph[srcNodeId] = MyCallGraphObj();
    MyCallGraphObj& srcObj = myGraph[srcNodeId];
    if (srcObj.dstNodes.find(dstNodeId) == srcObj.dstNodes.end())
        srcObj.dstNodes.insert(dstNodeId);

    if (myGraph.find(dstNodeId) == myGraph.end())
        myGraph[dstNodeId] = MyCallGraphObj();
    MyCallGraphObj& dstObj = myGraph[dstNodeId];
    dstObj.callers++;
}

/*!
 * Initialize sources
 */
void CGraph::initSrcs() {}

/*!
 * Initialize sinks
 */
void CGraph::initSnks() {}

void CGraph::reportBug(ProgSlice* slice) {}

void CGraph::finalize() {
    SrcSnkDDA::finalize();

    const SVFG *svfg = getSVFG();
    const PTACallGraph *ptaCallGraph = svfg->getPTACallGraph();

    for (auto i = ptaCallGraph->begin(); i != ptaCallGraph->end(); ++i) {
        const PTACallGraphNode *node = i->second;

#ifdef CGRAPH_DEBUG
        errs() << node->getFunction()->getName() << "\n";
#endif

        for (auto j = node->InEdgeBegin(); j != node->InEdgeEnd(); ++j) {
            const PTACallGraphEdge* edge = *j;
            NodeID srcNodeId = edge->getSrcID();
            NodeID dstNodeId = edge->getDstID();

            insertCall(srcNodeId, dstNodeId);

#ifdef CGRAPH_DEBUG
            const PTACallGraphNode *srcNode =
                    ptaCallGraph->getCallGraphNode(srcNodeId);
            const PTACallGraphNode *dstNode =
                    ptaCallGraph->getCallGraphNode(dstNodeId);
            errs() << "  " << srcNode->getFunction()->getName()
                   << " -> " << dstNode->getFunction()->getName() << "\n";
#endif
        }
    }

    for (auto i = myGraph.begin(); i != myGraph.end(); ++i) {
        NodeID srcNodeId = i->first;
        MyCallGraphObj srcObj = i->second;

        const PTACallGraphNode *srcNode =
                ptaCallGraph->getCallGraphNode(srcNodeId);

        std::cout << "Call graph node for function: '"
                  << srcNode->getFunction()->getName().str() << "'"
                  << "  " << "#callers=" << srcObj.callers << endl;;

        for (auto j = srcObj.dstNodes.begin();
             j != srcObj.dstNodes.end(); ++j) {
            NodeID dstNodeId = *j;

            const PTACallGraphNode *dstNode =
                    ptaCallGraph->getCallGraphNode(dstNodeId);

            std::cout << "  " << "Calls function '"
                      << dstNode->getFunction()->getName().str() << "'" << endl;
        }

        std::cout << endl;
    }
}

#endif /* MINION_CGRAPH */
