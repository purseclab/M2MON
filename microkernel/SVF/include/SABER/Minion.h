//===- Minion.h -- Minion analysis pass---------------------------------------//
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
 * Minion.h
 *
 *  Created on: Jul 1, 2017
 *      Author: Chung Hwan Kim
 */
#ifndef MINION_H_
#define MINION_H_

#include "SABER/SrcSnkDDA.h"
#include "SABER/SaberCheckerAPI.h"

/*!
 * Minion Analysis Pass
 */
class Minion : public SrcSnkDDA, public llvm::ModulePass {

public:
    /// Pass ID
    static char ID;

    /// Constructor
    Minion(char id = ID): ModulePass(ID) {
    }
    /// Destructor
    virtual ~Minion() {
    }

    /// We start from here
    virtual bool runOnModule(llvm::Module& module) {
        this->mod = &module;
        /// start analysis
        analyze(module);
        return false;
    }

    virtual void finalize();

    /// Get pass name
    virtual llvm::StringRef getPassName() const {
        return "Minion Analysis Pass";
    }

    /// Pass dependence
    virtual void getAnalysisUsage(llvm::AnalysisUsage& au) const {
        /// do not intend to change the IR in this pass,
        au.setPreservesAll();
    }

    /// Initialize sources and sinks
    //@{
    /// Initialize sources and sinks
    virtual void initSrcs();
    virtual void initSnks();
    /// Whether the function is a heap allocator/reallocator (allocate memory)
    virtual inline bool isSourceLikeFun(const llvm::Function* fun) {
        return true;
    }
    /// Whether the function is a heap deallocator (free/release memory)
    virtual inline bool isSinkLikeFun(const llvm::Function* fun) {
        return true;
    }
    /// A SVFG node is source if it is an actualRet at malloc site
    inline bool isSouce(const SVFGNode* node) {
        return getSources().find(node)!=getSources().end();
    }
    /// A SVFG node is source if it is an actual parameter at dealloca site
    inline bool isSink(const SVFGNode* node) {
        return getSinks().find(node)!=getSinks().end();
    }
    //@}

protected:
    /// Get PAG
    PAG* getPAG() const {
        return PAG::getPAG();
    }
    /// Report "points-to" analysis results
    //@{
    virtual void reportBug(ProgSlice* slice);

private:
    llvm::Module* mod;
};

#endif /* MINION_H_ */
