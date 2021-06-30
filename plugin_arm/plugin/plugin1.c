#define v493
#include <cstddef>
#include <gcc-plugin.h>
#include "plugin.h"
#include "plugin-version.h"
#include "tree-core.h"
#include "tree.h"
#include <string>
#include <sstream>
#ifndef v493
#include "errors.h"
#endif 
#include "gimple.h"
#include "rtl.h"
#include "emit-rtl.h"
#include "gimple-pretty-print.h"
#include "dumpfile.h"
#include "gimple-iterator.h"
#include "tree-pass.h"
#include "tree.h"
#include "context.h"
#include "tree-ssa-operands.h"
#include "tree-phinodes.h"
#include "gimple-ssa.h"
#include "ssa-iterators.h"
#include "ssa.h"
#include "langhooks.h"
#include "gimple-builder.h"
#include <iostream>
#include <fstream>
#include "cgraph.h"
#include <string>
//#define DEBUG_PRINT
#ifdef DEBUG_PRINT
#define DEBUG(format, arg...) printf(format, ##arg)
#else
#define DEBUG(x...)
#endif
std::ofstream ofs;
using namespace std;
int plugin_is_GPL_compatible;
void init_attributes(void);
#include <iostream>
#include <map>
using namespace std;
int init = 0;
map<string, int> sandboxHash;
map<tree, tree> pointersHash;
string databaseName;
void saveFunc(string name) {
	if (sandboxHash.count(name) == 0)
	{
		sandboxHash.insert(pair<string,int>(name,1));
		if(ofs.is_open())
		{
			ofs<<name<<endl;
		}
	}
}

bool isSandbox(string name) {
return (bool) sandboxHash.count(name);
}
unsigned long long totalRef;
unsigned long long instrRef;
unsigned long long instrAdded;
unsigned long long callsAdded;

/* Function to handle this attribute.  NODE points to the node to which
 the attribute is to be applied.  If a DECL, it should be modified in
 place; if a TYPE, a copy should be created.  NAME is the name of the
 attribute (possibly with leading or trailing __).  ARGS is the TREE_LIST
 of the arguments (which may be NULL).  FLAGS gives further information
 about the context of the attribute.  Afterwards, the attributes will
 be added to the DECL_ATTRIBUTES or TYPE_ATTRIBUTES, as appropriate,
 unless *NO_ADD_ATTRS is set to true (which should be done on error,
 as well as in any other cases when the attributes should not be added
 to the DECL or TYPE).  Depending on FLAGS, any attributes to be
 applied to another type or DECL later may be returned;
 otherwise the return value should be NULL_TREE.  This pointer may be
 NULL if no special handling is required beyond the checks implied
 by the rest of this structure.  */
tree attr_handler(tree *node, tree name, tree args, int flags,
	bool *no_add_attrs) {

/* warning, error, and fatal.  These definitions are suitable for use
 in the generator programs; eventually we would like to use them in
 cc1 too, but that's a longer term project. */

// FIXME: fatal("This plugin is not ready ");
/*Next is the type of the parameters. The function always take a fixed number of parameters. Since we don't have any parameters in the current example, we are invoking the function, tree_cons as shown. The first field is a purpose field and the last one is a chain field (for linking together different types like integers). We will be explaining more about this when we pass some parameters. */

//param_type_list = tree_cons(NULL_TREE, void_type_node, NULL_TREE); Failed attempt
gcc_assert(TREE_CODE(*node) == FUNCTION_DECL);

/* This should be a FUNCTION_DECL tree */
tree id = (DECL_NAME(*node));
/* Get name of the identifier tree */
DEBUG("Function registered: %s \n",
		(id ? IDENTIFIER_POINTER(id) : "<unnamed>"));

saveFunc(IDENTIFIER_POINTER(id));

return (NULL_TREE);
}

void plugin_func(void *gcc_data, void *user_data) {
/* Points to the current statement being walked.  */
gimple_stmt_iterator gsi;
basic_block bb = NULL;
if (cfun)

FOR_EACH_BB_FN(bb, cfun)
{
	for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
//		if (dump_file)
//			print_gimple_stmt(dump_file, gsi_stmt(gsi), 0, 0);
	}
}
}

void plugin_attr_func(void *gcc_data, void *user_data) {
static bool registered = false;
struct attribute_spec *attr;
char * name;
//  TODO: Version checking if (!plugin_default_version_check (version, &gcc_version))
//    return 1;

if (!registered) {
	registered = true;
	/*
	 OK, call into GCC to register the attribute.

	 (register_attribute doesn't have a return value; failures appear
	 to be fatal)
	 */

	/*
	 "struct attribute_spec" is declared in gcc/tree.h

	 register_attribute() is called by GCC for various attrs stored in
	 tables of global data e.g.:
	 const struct attribute_spec lto_attribute_table[]

	 Hence we must malloc the data, so that it persists for the rest of the
	 lifetime of the process

	 We get a "handler" callback, it gets passed the name of the attribute,
	 so maybe we can map names to callables.
	 */
	attr = (struct attribute_spec *) xmalloc(sizeof(struct attribute_spec));
	name = (char *) xmalloc(10);
	//strcpy(name, "irqbox");
	strcpy(name, "irqbox");

	attr->name = name;
	attr->min_length = 0;
	attr->max_length = 1;
	attr->decl_required = 0;
	attr->type_required = 0;
	attr->function_type_required = 0;
	attr->handler = attr_handler;

	/* Register attribute with GCC */
	register_attribute(attr);
}
}

const pass_data pass_data_test_show_locus = { GIMPLE_PASS, /* type */
"test_show_locus", /* name */
OPTGROUP_NONE, /* optinfo_flags */
TV_NONE, /* tv_id */
PROP_ssa, /* properties_required */
0, /* properties_provided */
0, /* properties_destroyed */
0, /* todo_flags_start */
0, /* todo_flags_finish */
};

#if 0
/* Metadata for a pass, non-varying across all instances of a pass.  */
struct pass_data
{
  /* Optimization pass type.  */
  enum opt_pass_type type;

  /* Terse name of the pass used as a fragment of the dump file
     name.  If the name starts with a star, no dump happens. */
  const char *name;

  /* The -fopt-info optimization group flags as defined in dumpfile.h. */
  unsigned int optinfo_flags;

  /* The timevar id associated with this pass.  */
  /* ??? Ideally would be dynamically assigned.  */
  timevar_id_t tv_id;

  /* Sets of properties input and output from this pass.  */
  unsigned int properties_required;
  unsigned int properties_provided;
  unsigned int properties_destroyed;

  /* Flags indicating common sets things to do before and after.  */
  unsigned int todo_flags_start;
  unsigned int todo_flags_finish;
};
#endif 

const pass_data pass_data_safe_stack = { RTL_PASS, /* type */
"safe_stack", /* name */
OPTGROUP_NONE, /* optinfo_flags */
TV_NONE, /* tv_id */
(PROP_ssa | PROP_gimple_leh | PROP_cfg
    | PROP_gimple_lcx
    | PROP_gimple_lvec
    | PROP_gimple_lva), /* properties_required */
0, /* properties_provided */
0, /* properties_destroyed */
0, /* todo_flags_start */
0, /* todo_flags_finish */
};
const pass_data pass_data_pro_epi_fixup = { RTL_PASS, /* type */
"pro_epi_fixup", /* name */
OPTGROUP_NONE, /* optinfo_flags */
TV_NONE, /* tv_id */
(PROP_ssa | PROP_gimple_leh | PROP_cfg
    | PROP_gimple_lcx
    | PROP_gimple_lvec
    | PROP_gimple_lva), /* properties_required */
0, /* properties_provided */
0, /* properties_destroyed */
0, /* todo_flags_start */
0, /* todo_flags_finish */
};

class pass_test_show_locus : public gimple_opt_pass
{
public:
pass_test_show_locus(gcc::context *ctxt)
: gimple_opt_pass(pass_data_test_show_locus, ctxt)
{}

/* opt_pass methods: */
bool gate (function *) {return true;}
virtual unsigned int execute (function *);

}; // class pass_test_show_locus
int seq_created;
rtx ref;
rtx prologue_sequence;
rtx epilogue_sequence;

/* Best way to write a pass, copy skeleton of existing pass */
class pass_pro_epi_fixup : public rtl_opt_pass
{
  public:
  pass_pro_epi_fixup(gcc:: context * ctxt): rtl_opt_pass(pass_data_pro_epi_fixup, ctxt)
  {
	  /* Stack pointer is not available untill init_emit_regs is called */
#if 0
	  rtx stack_var = gen_rtx_SYMBOL_REF(Pmode, "levaithan");
	  prologue_sequence = gen_rtx_SET(stack_pointer_rtx,(gen_rtx_MEM(Pmode,stack_var)));
	  epilogue_sequence = gen_rtx_SET(stack_pointer_rtx,(gen_rtx_MEM(Pmode,stack_var)));
#endif
  }

  /* opt_pass methods: */
  bool gate (function *) {return true;}
  virtual unsigned int execute (function *);

};

void fixup(function * current)
{
	rtx_insn * insn,last;
			int i =0;
			for (insn=get_insns();insn!=get_last_insn(); insn=NEXT_INSN(insn))
			{
				rtx op0 = XEXP (insn, 0); /* Get  expr_list from insn */
				rtx op1 = XEXP (insn, 1);
				rtx op3 = XEXP (insn, 3);
				rtx op6 = XEXP (insn, 6);
				if (GET_CODE(insn) == INSN) {

					rtx seq = XEXP (op6, 0); /* Get sequence from expr_list */
					rtx op3 = XEXP (insn, 3);
					rtvec ivec = XVEC(seq,0);
					rtvec ivecp = XVEC(op3,0);

					XEXP(XVEC(XEXP(XEXP(insn, 6),0),0)->elem[ivec->num_elem-1],1) = gen_rtx_REG (SImode, 12);
					break;
				}
			}
			char pinst = false;
			for(insn=get_insns();insn!=get_last_insn(); insn=NEXT_INSN(insn))
			{

				/* Find epilogue */
				if (GET_CODE(insn) == NOTE) {
					rtunion note_data = NOTE_DATA(insn);
					enum insn_note note = (enum insn_note)NOTE_KIND(insn);
				    if(note == NOTE_INSN_EPILOGUE_BEG)
				    {
				    	/* Move to pop instruction */
				    	while (GET_CODE(insn) != JUMP_INSN)
                        insn =  NEXT_INSN(insn);
                        rtx op0 = XEXP (insn, 0);
                        				rtx op1 = XEXP (insn, 1);
                        				rtx op3 = XEXP (insn, 3);
                        				rtx op6 = XEXP (insn, 6);
                        				{
                        					/* Get  expr_list from insn */
                        					if (op3 ==0) break;
                        					rtx sq = XEXP (op3, 0);


                        					/* Always the last instruction is pushing LR, i.e. SET */
                        					rtvec ivec = XVEC(op3,0);

                        					//XEXP(XEXP(XEXP(XEXP(insn, 6),0),0),2) = gen_nop();
                        					//XEXP(XVEC(XEXP(XEXP(insn, 6),0),0)->elem[ivecs->num_elem-1],1) = gen_rtx_REG (SImode, 12);
                        					//XEXP(XVEC(XEXP (insn, 3),0)->elem[ivec->num_elem-1],1) = gen_rtx_REG (SImode, LR_REGNUM);
                        					rtx update_stk = gen_rtx_SET(XEXP(XVEC(XEXP (insn, 3),0)->elem[ivec->num_elem-1],1),
                        							gen_rtx_REG (SImode, LR_REGNUM));
                        					emit_insn_before(update_stk, insn);
                        					//XEXP(XEXP(XEXP(XEXP(insn, 3),0),4),0) = gen_rtx_REG (SImode, 12);
                        					pinst = true;
                        					break;
                        				}
				    	break;
				    }
				}
			}

			if (pinst == false)
			{

			}


}

unsigned int
pass_pro_epi_fixup::execute (function *fun)
{
  //name is stored in ((((((((((((fun)->decl))->decl_common)).common)).name))->identifier)).id)
  if (fun)
  {
  if(isSandbox(IDENTIFIER_POINTER(DECL_NAME(fun->decl))))
  	{
  		fixup(fun);
  	}
  }

	return 0;
}




/* Best way to write a pass, copy skeleton of existing pass */
class pass_safe_stack : public rtl_opt_pass
{
  public:
  pass_safe_stack(gcc:: context * ctxt): rtl_opt_pass(pass_data_safe_stack, ctxt)
  {
	  /* Stack pointer is not available untill init_emit_regs is called */
#if 0
	  rtx stack_var = gen_rtx_SYMBOL_REF(Pmode, "levaithan");
	  prologue_sequence = gen_rtx_SET(stack_pointer_rtx,(gen_rtx_MEM(Pmode,stack_var)));
	  epilogue_sequence = gen_rtx_SET(stack_pointer_rtx,(gen_rtx_MEM(Pmode,stack_var)));
#endif
  }

  /* opt_pass methods: */
  bool gate (function *) {return true;}
  virtual unsigned int execute (function *);

};

void safeguard(function * current)
{
//	if (!seq_created)
//	{
//		//gen_rtx_MEM(Pmode, stack_var)
//		rtx stack_var = gen_rtx_SYMBOL_REF(Pmode, "shadowStack");
//		SYMBOL_REF_FLAGS (stack_var) |= SYMBOL_FLAG_LOCAL;
//		//SYMBOL_REF_DATA (x) = SYMBOL_REF_DECL;
//		rtx temp = gen_rtx_REG(Pmode, LAST_VIRTUAL_REGISTER + 1);
//		REG_POINTER(temp) = 1;
//		ref = gen_rtx_SET(temp, stack_var);
//		rtx mem= gen_rtx_MEM(Pmode,temp);
//		mem->call = 1;
//		prologue_sequence = gen_rtx_SET(mem, gen_rtx_REG (SImode, LR_REGNUM));
//		epilogue_sequence = gen_rtx_SET(return_address_pointer_rtx,
//				(gen_rtx_MEM(Pmode, stack_var)));
//		seq_created = 1;
//	}
//	rtx_insn * insn,last;
//	for (insn=get_insns();insn!=get_last_insn(); insn=NEXT_INSN(insn))
//	{
//		int is_insn;
//		is_insn = 0;
//
//		if (GET_CODE(insn) != NOTE)
//		{
//
//		emit_insn_before(ref ,emit_insn_before (prologue_sequence, insn));
//
//		//rtx_insn *  instr = emit_insn_before (ref, insn);
//		//emit_insn_after(prologue_sequence,instr);
//
//			//emit_insn_before(prologue_sequence, insn);
//		break;
//		}
//		//print_rtl_single(0)
//	}
//	insn = get_last_insn();
//	emit_insn_before(gen_nop(), insn);
	rtx_insn * insn, last;
	int i = 0;
	for (insn = get_insns(); insn != get_last_insn(); insn = NEXT_INSN(insn)) {
		int is_insn;
		is_insn = 0;
		rtx op0 = XEXP(insn, 0);
		rtx op1 = XEXP(insn, 1);
		rtx op3 = XEXP(insn, 3);
		rtx op6 = XEXP(insn, 6);
//		if (GET_CODE(insn) == NOTE) {
//			rtunion note_data = NOTE_DATA(insn);
//			enum insn_note note = (enum insn_note) NOTE_KIND(insn);
//
//			if (note == NOTE_INSN_FUNCTION_BEG) {
//				for (int i = 0; i <5 ; i++) // move 5 instructions
//                insn = NEXT_INSN(insn);
				if (GET_CODE(insn) == INSN) {

					if (GET_CODE(op3) == SET) {
						rtx r0 = XEXP(op3, 0);
						rtx r1 = XEXP(op3, 1);
						if (GET_CODE(r0) == REG) {
							tree id0 = REG_EXPR(r0);
							if (id0 && TREE_CODE(id0) != RESULT_DECL && DECL_NAME(id0)) {
								if (strncmp("SLR",
										IDENTIFIER_POINTER(DECL_NAME(id0)), 3)
										== 0)
									XEXP(XEXP(insn, 3), 0) = gen_rtx_REG(SImode,
											LR_REGNUM);
							}
						}
						if (GET_CODE(r1) == REG) {
							tree id1 = REG_EXPR(r1);
							if (id1 && TREE_CODE(id1) != RESULT_DECL && DECL_NAME(id1)) {
								if (strncmp("SLR",
										IDENTIFIER_POINTER(DECL_NAME(id1)), 3)
										== 0)
									XEXP(XEXP(insn, 3), 1) = gen_rtx_REG(SImode,
											LR_REGNUM);
							}
						}
					}
				}
//			}
//		}
	}
}

unsigned int
pass_safe_stack::execute (function *fun)
{
  //name is stored in ((((((((((((fun)->decl))->decl_common)).common)).name))->identifier)).id)
  if (fun)
  {
  if(isSandbox(IDENTIFIER_POINTER(DECL_NAME(fun->decl))))
  	{
  		safeguard(fun);
  	}
  }

	return 0;
}



static gimple_opt_pass *
make_pass_test_show_locus (gcc::context *ctxt)
{
return new pass_test_show_locus (ctxt);
}

static rtl_opt_pass *
make_pass_safe_stack (gcc::context *ctxt)
{
return new pass_safe_stack (ctxt);
}

static rtl_opt_pass *
make_pass_pro_epi_fixup (gcc::context *ctxt)
{
return new pass_pro_epi_fixup (ctxt);
}

tree helpers[10];
void init_builtins()
{
	tree BT_FN_VOID_CONST_PTR_INT
	    = build_function_type_list (void_type_node, const_ptr_type_node,
	                                integer_type_node, NULL_TREE);
	helpers[0] = add_builtin_function ("my_helper", BT_FN_VOID_CONST_PTR_INT, 0, BUILT_IN_NORMAL, NULL,
	    NULL_TREE /* attrs */);
}

void sanitize(function * current) {
/* Points to the current statement being walked.  */
gimple_stmt_iterator gsi;
basic_block bb = NULL;
tree  mmio_pointer;
use_operand_p imm_use;
gimple *use_stmt;
volatile tree use_lhsop;
volatile tree use_rhsop1;
volatile tree use_rhsop2;
volatile tree use_rhsop3;
int once = 0;

FOR_EACH_BB_FN(bb, current)
{
	for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); ) {

//		/* Dump Gimple */
////		if (dump_file)
////			print_gimple_stmt(dump_file, gsi_stmt(gsi), 0, 0);
////
		/* Req1: No Intermodule Calls */
		if (is_gimple_call(gsi_stmt(gsi))) {

			gimple * stmt = gsi_stmt(gsi);
			tree lhsop = gimple_call_lhs(stmt);
			enum internal_fn fn = gimple_call_internal_fn(stmt);
			tree fntype = gimple_call_fntype(stmt);
			/* Get the function called */
			tree fun_called = gimple_call_fn(stmt);

			/* See if this is an address expression, IAA ADDR_EXPR is label type expressions */
			if (TREE_CODE(fun_called) != ADDR_EXPR){
				/* Get the defining statment of this call, most like a pointer function */
				gimple * def_stmt = fun_called->ssa_name.def_stmt;

				/* Definition should be an assign */
				if (is_gimple_assign(def_stmt))
				{
					/* Get operands of the assignment */
					tree lhsop = gimple_assign_lhs(def_stmt);
			        tree rhsop1 = gimple_assign_rhs1(def_stmt);
			        const char* name = IDENTIFIER_POINTER (rhsop1);
			        const char* name2 = IDENTIFIER_POINTER (lhsop);
			        tree id = SSA_NAME_IDENTIFIER(lhsop);
			        if (id)
			        {
			        const char* name3 = IDENTIFIER_POINTER(id);
			        std::ofstream ofs2;
			        ofs.open ("/home/arslan/pointer_funcs", std::ios_base::app);
			        ofs2 <<"Indirect write using pointer:"<< name3 <<endl<<flush;
			        }

			        rhsop1 = rhsop1;
			        int i =0;
			        if (rhsop1) i++;
				}
				ssa_op_iter dit;
				def_operand_p def_p;
				FOR_EACH_SSA_DEF_OPERAND (def_p, stmt, dit, SSA_OP_DEF)
				              {
						tree odef = DEF_FROM_PTR(def_p);
						odef = odef;
					}
				DEBUG("%s can't have calls", IDENTIFIER_POINTER(DECL_NAME(cfun->decl)));
				//gcc_assert(0);
			}
		}

		//print_gimple_stmt(stderr,gsi_stmt(gsi),0,0);
		/* Req2: All IO should go through provided gateway */
		if (is_gimple_assign(gsi_stmt(gsi))) {
			gimple * stmt = gsi_stmt(gsi);
			int field_access = 0;
			tree lhsop = gimple_assign_lhs(stmt);
		    tree rhsop1 = gimple_assign_rhs1(stmt);

			if (TREE_CODE(lhsop) == SSA_NAME)
			{
				DEBUG("pointer manipulated");
			}
			if (TREE_CODE(lhsop) == COMPONENT_REF)
			{
				/* According to tree.def it has 3 operands */
				tree gstruct = TREE_OPERAND (lhsop, 0);
				tree field =  TREE_OPERAND (lhsop, 1);//FIELD_DECL
				tree field_off = TREE_OPERAND (lhsop, 2);//DECL_FIELD_OFFSET, measured in units of DECL_OFFSET_ALIGN / BITS_PER_UNIT.
				field_access = int_cst_value(field->field_decl.common.size);
				lhsop = gstruct;
			}

			if (TREE_CODE(lhsop) == MEM_REF || TREE_CODE(rhsop1) == MEM_REF) {
			    DEBUG("Memory Reference");

			    /* Alias analysis was always returning false positives */
//			    if (refs_may_alias_p(lhsop,use_lhsop))
//			    	cout << "Alias found" <<endl;

			    // THis is a better way than creating a database map.
			    tree temp;
			    if (TREE_CODE(rhsop1) == MEM_REF)
			    {
			    	/* TODO: FIXME This is a temp hack to get numbers modify code down to fix */
			        temp = TREE_OPERAND (rhsop1, 0);
			    }
			    else
			    {
			    	temp = TREE_OPERAND (lhsop,0);
			    }
			    int i = 1;
			    //ppg.1_10 = ppg; Local with global
			    gimple * def_stmt = SSA_NAME_DEF_STMT(temp);
			    tree def_lhsop = NULL;
			    tree def_rhsop1 = NULL;
			    if (is_gimple_assign(def_stmt)) //TODO:Add support for GIMPLE_ASM
			    {
			    def_lhsop = gimple_assign_lhs(def_stmt);
			    def_rhsop1 = gimple_assign_rhs1(def_stmt);
			    }
			    while(def_rhsop1) //TODO:Add support for GIMPLE_ASM
			    {
			    	if (TREE_CODE((def_rhsop1)) == INTEGER_CST)
			    		break;
			    	// See if global variable, this is dangerous because even though defined it can be changed..
			    	if (TREE_CODE(def_rhsop1) == VAR_DECL)
			    	{
			    		// According to tree.def VAR_DECL has no operands
			    		struct symtab_node * sn = symtab_node::get(def_rhsop1);
			    		struct varpool_node * vn = varpool_node::get(def_rhsop1);
			    		struct cgraph_node * cn = cgraph_node::get(def_rhsop1);

			    		if (!vn) {
			    			/* TODO: add a check with a leviathan root string comparison
			    			 * Probably one of our generated symbol */
			    			def_rhsop1 = NULL;
			    			break;
			    		}

			    		// See if variable was initialized in this TU
			    		if (DECL_INITIAL(vn->decl))
			    		{
			    			//Code copied from varpool_node::dump by analyzing pass test.c.000i.cgraph
			    			tree decl = DECL_INITIAL(vn->decl);
			    			//cout<<sn->name() << sn->asm_name() <<endl;
			    			def_rhsop1 = decl;
			    			break;
			    		}
			    		symtab_node::debug_symtab();
			    	}
			    	def_stmt = SSA_NAME_DEF_STMT(def_rhsop1);
			    	if (is_gimple_assign(def_stmt))
			    	{
			    		def_rhsop1 = gimple_assign_rhs1(def_stmt);
			    	}
			    	else
			    	{
			    		def_rhsop1 = NULL;
			    		break;
			    	}
			    }

			    /* Write operation */
			    if (def_rhsop1 && TREE_CODE((def_rhsop1)) == INTEGER_CST)
			    {
			    	//gimple * def_stmt = SSA_NAME_DEF_STMT(lhsop);

			    	// THis is a better way than creating a database map.
//			    	tree temp = TREE_OPERAND (lhsop, 0);
//			    	gimple * def_stmt = SSA_NAME_DEF_STMT(temp);
//			    	tree def_lhsop = gimple_assign_lhs(def_stmt);
//			    	tree def_rhsop1 = gimple_assign_rhs1(def_stmt);
			    	tree def_lhsop = temp;
			    	tree addr = def_lhsop;
			    	tree pointer_node = TREE_OPERAND(lhsop, 0);

			    	tree lhsop_points_to = TREE_TYPE(lhsop);
			    	/* Size of access is in lhsop_points_to.type_common.size.int_cst.val[0] */
			    	HOST_WIDE_INT size = int_cst_value(TYPE_SIZE(lhsop_points_to));
			    	if (field_access) size = field_access;
			    	int index = size/16 + 0;
			    	lhsop_points_to = TYPE_SIZE(addr);
			    	lhsop_points_to = TYPE_SIZE(pointer_node);

			    	HOST_WIDE_INT bitsize, bitpos;
			    	tree offset;
			    	machine_mode mode;
			    	int unsignedp, reversep, volatilep = 0;

			    	if (helpers[0] == NULL)
			    		init_builtins();


			    	tree mem_ref = gimple_assign_lhs (stmt);
			    	//if (TREE_CODE(mem_ref) == COMPONENT_REF) mem_ref = TREE_OPERAND (lhsop, 0);  //TODO: Fix this hack
			    	tree val = gimple_assign_rhs1 (stmt);
			    	tree inner_ref = get_inner_reference (mem_ref, &bitsize, &bitpos, &offset, &mode,
			    				    	                 &unsignedp, &reversep, &volatilep, false);

			    	volatile tree inner_ref2 = inner_ref;
			    	if (field_access) addr = build1(ADDR_EXPR,TREE_TYPE(gimple_assign_lhs(stmt)),gimple_assign_lhs(stmt));

			    	gimple *g = gimple_build_call (helpers[index], 2, val, addr);
			    	callsAdded++;
			    	gimple_set_location (g, gimple_location (stmt));
			    	//gsi_insert_before (&gsi, g, GSI_SAME_STMT);
			    	//gsi_remove (&gsi, true);
			    	 /* If we do not have useful points-to information for this pointer
			    	     we cannot disambiguate anything else.  */
			    	struct ptr_info_def *pi;
			    	pi = SSA_NAME_PTR_INFO (lhsop);
			    	bool pointsTo;
			    	if (pi)
			    		  pointsTo = pt_solution_includes (&pi->pt, rhsop1); // this should be decl
			    	gsi_replace(&gsi, g, true); //Call disabled
			    	cout<<"Call Added" << endl <<endl << endl;
			    	gsi_next(&gsi); /* Remove this line before commenting upper block */
			    	continue;
			    }

			    /* Read operation */
				if (pointersHash.count(TREE_OPERAND(rhsop1, 0)) == 1) {
					tree addr = pointersHash.find(TREE_OPERAND(rhsop1, 0))->second;
					tree rhsop1_points_to = TREE_TYPE(rhsop1);
					/* Size of access is in lhsop_points_to.type_common.size.int_cst.val[0] */
					HOST_WIDE_INT size = int_cst_value(TYPE_SIZE(rhsop1_points_to));
					HOST_WIDE_INT bitsize, bitpos;
					tree offset;
					machine_mode mode;
					int unsignedp, reversep, volatilep = 0;
					int index = size/16 + 3;

					if (helpers[1] == NULL)
						init_builtins();

					tree mem_ref = gimple_assign_lhs(stmt);
					tree val = gimple_assign_rhs1(stmt);

					tree inner_ref = get_inner_reference(mem_ref, &bitsize,
							&bitpos, &offset, &mode, &unsignedp, &reversep,
							&volatilep, false);

					volatile tree inner_ref2 = inner_ref;

					//tree tmp0 = create_tmp_var (TREE_TYPE(TREE_OPERAND(rhsop1, 0)), "leviathan");
					gimple *g = gimple_build_call(helpers[index], 1, addr);
					callsAdded++;
					cout<<"Call Added" << endl <<endl << endl;

					gimple_call_set_lhs(g,lhsop);
					gimple_set_location(g, gimple_location(stmt));
					//gsi_insert_before(&gsi, g, GSI_SAME_STMT);

					gsi_replace(&gsi, g, true); //Call disabled
					//gimple_set_op (stmt, 1, tmp0);
					gsi_next(&gsi);
					continue;
				}

			}
			if (POINTER_TYPE_P(TREE_TYPE (lhsop))) {
				if (TREE_CODE((rhsop1)) == INTEGER_CST)
				{
					DEBUG("MMIO Pointer Create");
					// This is debug code
//				int val = ((TREE_INT_CST_HIGH (rhsop1) << HOST_BITS_PER_WIDE_INT)
//				+ TREE_INST_CST_LOW (rhsop1));
//
//				    cout << val << endl;

					/*
					 * This was done for alias analysis.
					 */

//				    if (single_imm_use(lhsop, &imm_use, &use_stmt) && !once)
//				    {
//				    	mmio_pointer =   lhsop;
//				    	use_lhsop = gimple_assign_lhs(use_stmt);
//				    	use_rhsop1 = gimple_assign_rhs1(use_stmt);
//				    	once = 1;
//				    }


				    /*
				     * Cannot use FOR_EACH_SSA_USE_OPERAND (use_p, stmt, iter, (SSA_OP_USE | SSA_OP_VUSE)) since modifying
				     * statements causes the iterator to malfunction.
				     */
				    pointersHash.insert(pair<tree,tree>(lhsop,rhsop1));
				}

				    gsi_next(&gsi);
				    continue;
			}

			/* Req3: All stores should remain inside the IRQ stack/data section */
			if (gimple_store_p (gsi_stmt(gsi)))
			{
				gimple * stmt = gsi_stmt(gsi);
				volatile tree lhsop = gimple_assign_lhs(stmt);
				switch (TREE_CODE(lhsop)) {
				case ARRAY_REF:
				case COMPONENT_REF:
				case INDIRECT_REF:
				case MEM_REF:
				case BIT_FIELD_REF:
					break;
					/* FALLTHRU */
				default:
					gsi_next(&gsi);
					continue;
				}
				instrRef++;
				HOST_WIDE_INT bitsize, bitpos;
				tree offset;
				machine_mode mode;
				int unsignedp, reversep, volatilep = 0;
				location_t loc = gimple_location(stmt);

				tree inner = get_inner_reference (lhsop, &bitsize, &bitpos, &offset, &mode,
				                                    &unsignedp, &reversep, &volatilep, false);
				if(is_global_var(lhsop))
				{
					DEBUG("Global Variable Access");
				}

//				gimple_alloc_stat
				tree cast_type = build_distinct_type_copy(unsigned_type_node);
				tree original_type = build_pointer_type(TREE_TYPE(gimple_assign_lhs(stmt)));
				inner = TREE_OPERAND(gimple_assign_lhs(stmt), 0);
				if (TREE_CODE(inner) == MEM_REF)
				{
					inner = TREE_OPERAND(inner, 0);
				}
				if (!POINTER_TYPE_P(TREE_TYPE(inner)) )
				{
				    gsi_next(&gsi);
				    continue;
				}
				tree * tmp0 = (tree *)xmalloc(sizeof(tree));
				*tmp0 = create_tmp_var (cast_type,"leviathan");
				//tree tmp0=get_initialized_tmp_var()
				TREE_NO_WARNING(*tmp0) = 1;
				gimple *def_stmt =SSA_NAME_DEF_STMT(*tmp0);

				gimple *g =build_type_cast(cast_type, inner,*tmp0);
				gimple_set_location (g, loc);
				gimple_set_block (g, gimple_block (stmt));
				gsi_insert_before (&gsi, g, GSI_SAME_STMT);
				SSA_NAME_DEF_STMT(*tmp0) = g; // This is important or you will else get
/* GCC [C/C++ Application]
	arm-none-eabi-gcc [27286] [cores: 6]
		Thread #2 [cc1] 27286 [core: 6] (Suspended : Signal : SIGSEGV:Segmentation fault)
			gimple_code() at gimple.h:1,672 0xf2ffc9
			gimple_nop_p() at gimple.h:6,146 0xf30997
			ssa_undefined_value_p() at tree-ssa.c:1,190 0xf33ec8
			has_undefined_value_p() at tree-ssa-uninit.c:78 0xf28d80
			warn_uninit() at tree-ssa-uninit.c:133 0xf28ec4
			warn_uninitialized_vars() at tree-ssa-uninit.c:225 0xf29395
			execute_early_warn_uninitialized() at tree-ssa-uninit.c:2,514 0xf2de41
			(anonymous namespace)::pass_early_warn_uninitialized::execute at tree-ssa-uninit.c:2,550 0xf2deb1
			execute_one_pass() at passes.c:2,336 0xc06364
			execute_pass_list_1() at passes.c:2,420 0xc0669b
			execute_pass_list() at passes.c:2,431 0xc06722
			do_per_function_toporder() at passes.c:1,725 0xc04ced
			execute_ipa_pass_list() at passes.c:2,773 0xc072be
			ipa_passes() at cgraphunit.c:2,277 0x89eef4
			symbol_table::compile() at cgraphunit.c:2,416 0x89f2f6
			symbol_table::finalize_compilation_unit() at cgraphunit.c:2,576 0x89f69f
			compile_file() at toplev.c:488 0xd06f88
			do_compile() at toplev.c:2,011 0xd093b2
			toplev::main() at toplev.c:2,119 0xd09631
			main() at main.c:39 0x14f88f5
	<terminated>arm-none-eabi-gcc [27272]
	gdb (8.1.0.20180409)
 *
 * */

				//tree lhs_and = create_tmp_var(cast_type, "leviathan");
				tree op1 = build_int_cst(cast_type, ~(0<<28)); // Change it to 1 later on
				//g = build_assign(BIT_AND_EXPR, tmp0, op1, lhs_and);
				g = build_assign(BIT_AND_EXPR, *tmp0, op1, *tmp0);
				instrAdded++;
				gimple_set_location (g, loc);
				gimple_set_block (g, gimple_block (stmt));
				gsi_insert_before (&gsi, g, GSI_SAME_STMT);

				//tree lhs_or = create_tmp_var (cast_type, "leviathan");
				op1 = build_int_cst(cast_type, 0x0000000);
				//g = build_assign( BIT_IOR_EXPR, lhs_and, op1, lhs_or);
				g = build_assign( BIT_IOR_EXPR, *tmp0, op1, *tmp0);
				instrAdded++;
				gimple_set_location (g, loc);
				gimple_set_block (g, gimple_block (stmt));
				gsi_insert_before (&gsi, g, GSI_SAME_STMT);

				g =build_type_cast(original_type, *tmp0,inner);
				instrAdded++;
				gimple_set_location (g, loc);
				gimple_set_block (g, gimple_block (stmt));
				gsi_insert_before (&gsi, g, GSI_SAME_STMT);
			}

		}

		gsi_next(&gsi);
	}
}

FOR_EACH_BB_FN(bb, current)
{
	for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
      gimple * stmt = gsi_stmt(gsi);
      use_operand_p use_p;
	  ssa_op_iter iter;
	  print_gimple_stmt(stderr,gsi_stmt(gsi),0,0);
	  FOR_EACH_SSA_USE_OPERAND (use_p, stmt, iter, SSA_OP_USE)
      {
		  //print_gimple_stmt(stderr,gsi_stmt(gsi),0,0);
		  gimple *def_stmt = SSA_NAME_DEF_STMT (USE_FROM_PTR (use_p));
		  if (def_stmt == NULL)
			  while(1);

			  //print_gimple_stmt(stderr,gsi_stmt(gsi),0,0);
      }
	}
}
}
#define SSP_ADDR 0x10000000
void updateProEpi()
{
	gimple_stmt_iterator gsi;
	basic_block bb = NULL;
	tree  mmio_pointer;
	use_operand_p imm_use;
	gimple *use_stmt;
	volatile tree use_lhsop;
	volatile tree use_rhsop1;
	volatile tree use_rhsop2;
	volatile tree use_rhsop3;
	int once = 0;

	bb = cfun->cfg->x_entry_block_ptr->next_bb;
			gsi = gsi_start_bb(bb);
			tree * ssb = (tree *)xmalloc(sizeof(tree));

			tree * ref = (tree *)xmalloc(sizeof(tree));
			tree * ref1 = (tree *)xmalloc(sizeof(tree));

			// safeStackref = SSP_ADDR;
			*ssb = create_tmp_var(build_pointer_type(unsigned_intSI_type_node), "SSB");
			tree ssp = create_tmp_var(build_pointer_type(unsigned_intSI_type_node), "SSP");
			tree slr = create_tmp_var(build_pointer_type(unsigned_intSI_type_node), "SLR");

			tree op1 = build_int_cst(unsigned_intSI_type_node, SSP_ADDR);
			gimple *g = build_assign(INTEGER_CST, op1, NULL_TREE, *ssb);
			SSA_NAME_DEF_STMT(*ssb) = g;
			instrAdded++;
			gsi_insert_before (&gsi, g, GSI_SAME_STMT);


			// SSP = *SSB
			//*ref = build1(INDIRECT_REF, void_type_node, *tmp0);
	//		*ref = build1(INDIRECT_REF, void_type_node, *tmp0);
	//		//*ref = build2(MEM_REF, void_type_node, NULL_TREE, *tmp0);
	//		//TREE_TYPE(*ref) = uint32_type_node;
	//		//g = build_assign(INDIRECT_REF, *ref, NULL_TREE, *tmp0);
	//		g = build_assign(INDIRECT_REF, *ref, NULL_TREE, *tmp0);
	//		gsi_insert_before (&gsi, g, GSI_SAME_STMT);

			//This version gets optimized out
	//		*ref = build1(INDIRECT_REF, void_type_node, *tmp0);
	//		g = build_assign(INDIRECT_REF, *ref, NULL_TREE, *tmp0);
	//		gsi_insert_before (&gsi, g, GSI_SAME_STMT);



			// MEM_REF, type of ref, thing we are referencing, offset to the reference
			*ref = build2(MEM_REF, unsigned_intSI_type_node, *ssb, build_int_cst (ptr_type_node, 0));
			g = build_assign(INDIRECT_REF, *ref, NULL_TREE, ssp);
			instrAdded++;
			gsi_insert_before (&gsi, g, GSI_SAME_STMT);


			//SSP = SSP - 4;
			tree lrSize = build_int_cst(unsigned_intSI_type_node, 4);
			g = build_assign(MINUS_EXPR,ssp,lrSize,ssp);
			SSA_NAME_DEF_STMT(ssp) = g;
			instrAdded++;
			gsi_insert_before (&gsi, g, GSI_SAME_STMT);

			//Update SP in memory
			*ref1 = build2(MEM_REF, unsigned_intSI_type_node, *ssb, build_int_cst (ptr_type_node, 0));
			TREE_TYPE(*ref1) = uint32_type_node;
			g = build_assign(SSA_NAME, ssp, NULL_TREE, *ref1);
			instrAdded++;
			gsi_insert_before (&gsi, g, GSI_SAME_STMT);

			// *safeStackref = 0xdeadbeef
			//op1 = build_int_cst(unsigned_intSI_type_node, 0xdeadbeef);
			//*ref1 = build1(INDIRECT_REF, unsigned_intSI_type_node, *tmp0);
			//TREE_TYPE(*ref1) = uint32_type_node;
			//TREE_CODE(TREE_TYPE(op1))
			//TODO: See how this memref is made in code.
			*ref1 = build2(MEM_REF, unsigned_intSI_type_node, ssp, build_int_cst (ptr_type_node, 0));
			TREE_TYPE(*ref1) = uint32_type_node;
			g = build_assign(SSA_NAME, slr, NULL_TREE, *ref1);
			instrAdded++;
			gsi_insert_before (&gsi, g, GSI_SAME_STMT);


			bb = cfun->cfg->x_exit_block_ptr->prev_bb;
	        // Instrument epilogue
			gsi = gsi_last_bb(bb);

			/* Restore LR */
			g = build_assign(SSA_NAME, *ref1, NULL_TREE, slr);
			SSA_NAME_DEF_STMT(slr) = g;
			instrAdded++;
			gsi_insert_before (&gsi, g, GSI_SAME_STMT);


			//SSP = SSP + 4;
			g = build_assign(PLUS_EXPR,ssp,lrSize,ssp);
			instrAdded++;
			gsi_insert_before (&gsi, g, GSI_SAME_STMT);

			//*SSB = SSP
			g = build_assign(INDIRECT_REF, ssp, NULL_TREE, *ref);
			instrAdded++;
			gsi_insert_before (&gsi, g, GSI_SAME_STMT);
}
unsigned int
pass_test_show_locus::execute (function *fun)
{
if(cfun)
{
	/* Reference counting code */
	gimple_stmt_iterator gsi;
	basic_block bb = NULL;
	tree  mmio_pointer;
	use_operand_p imm_use;
	gimple *use_stmt;
	volatile tree use_lhsop;
	volatile tree use_rhsop1;
	volatile tree use_rhsop2;
	volatile tree use_rhsop3;
	int once = 0;
	if(isSandbox(IDENTIFIER_POINTER(DECL_NAME(cfun->decl))))
	{
		/* Fullfulls Requirements for SFI */
		sanitize(cfun);

		/* Implements SafeStack */
		updateProEpi();
	}

#if 00
	struct varpool_node *n;
	 FOR_EACH_VARIABLE(n)
	 {
		  // tree temp = DECL_NAME(n->decl);
           cout<<n->name()<<n->externally_visible<<n->definition<<endl;
	 }
	 cout << "functions..!!"<<endl;
	 cgraph_node * cn;
	 FOR_EACH_FUNCTION(cn){
		 cout<<cn->name()<<endl;
	 }
	 struct symtab_node * sn;
	 cout << "Symbols..!!" <<endl;
	FOR_EACH_DEFINED_SYMBOL(sn)
	 {
		cout << sn->name() <<endl;
	 }
#endif



	FOR_EACH_BB_FN(bb, cfun)
	{
	for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
		/* Req3: All stores should remain inside the IRQ stack/data section */
		if (is_gimple_assign(gsi_stmt(gsi)))
		{
					if (gimple_store_p (gsi_stmt(gsi)))
					{
						gimple * stmt = gsi_stmt(gsi);
						volatile tree lhsop = gimple_assign_lhs(stmt);
						if(lhsop)
						{
						switch (TREE_CODE(lhsop)) {
						case ARRAY_REF:
						case COMPONENT_REF:
						case INDIRECT_REF:
						case MEM_REF:
						case BIT_FIELD_REF:
							totalRef++;
							break;
							/* FALLTHRU */
						default:
							continue;
						}
						}
					}
				}
		}
	}
	std::ofstream ofs1("/home/arslan/indirect_references");
	if (ofs1.is_open())
	{
	ofs1<<totalRef<<endl;
	ofs1<<instrRef<<endl;
	ofs1<<instrAdded<<endl;
	ofs1<<callsAdded<<endl;
	//cout << totalRef <<endl <<instrRef <<endl;
	ofs1.close();
	}
	else
	{
		gcc_assert("Adios Muchacha");
	}
}
	/* Write back our iterators */
std::ofstream ofs1("/home/arslan/indirect_references");
	if (ofs1.is_open())
	{
	ofs1<<totalRef<<endl;
	ofs1<<instrRef<<endl;
	ofs1<<instrAdded<<endl;
	ofs1<<callsAdded<<endl;
	//cout << totalRef <<endl <<instrRef <<endl;
	ofs1.close();
	}
	else
	{
		gcc_assert("Adios Muchacha");
	}

return 0;
}

void initialize_builtins(void *gcc_data, void *user_data)
{
	tree BT_FN_VOID_CONST_PTR_LONG
	    = build_function_type_list (void_type_node, const_ptr_type_node, unsigned_type_node, NULL_TREE);

	tree BT_FN_VOID_CONST_PTR_SHORT
		    = build_function_type_list (integer_type_node, const_ptr_type_node, short_unsigned_type_node, NULL_TREE);

	tree BT_FN_VOID_CONST_PTR_CHAR
		    = build_function_type_list (void_type_node, const_ptr_type_node,unsigned_char_type_node, NULL_TREE);

	tree BT_FN_CHAR_CONST_PTR
		        = build_function_type_list (unsigned_char_type_node, const_ptr_type_node, NULL_TREE);
	tree BT_FN_SHORT_CONST_PTR
			    = build_function_type_list (short_unsigned_type_node, const_ptr_type_node, NULL_TREE);
	tree BT_FN_LONG_CONST_PTR
			    = build_function_type_list (unsigned_type_node, const_ptr_type_node, NULL_TREE);


	helpers[0] = add_builtin_function ("cputreg8", BT_FN_VOID_CONST_PTR_CHAR, 0, BUILT_IN_NORMAL, NULL,
	    NULL_TREE);
	helpers[1] = add_builtin_function ("cputreg16", BT_FN_VOID_CONST_PTR_SHORT, 0, BUILT_IN_NORMAL, NULL,
		    NULL_TREE);
	helpers[2] = add_builtin_function ("cputreg32", BT_FN_VOID_CONST_PTR_LONG, 0, BUILT_IN_NORMAL, NULL,
		    NULL_TREE);
	helpers[3] = add_builtin_function ("cgetreg8", BT_FN_CHAR_CONST_PTR, 0, BUILT_IN_NORMAL, NULL,
		    NULL_TREE);
	helpers[4] = add_builtin_function ("cgetreg16", BT_FN_SHORT_CONST_PTR, 0, BUILT_IN_NORMAL, NULL,
			    NULL_TREE);
	helpers[5] = add_builtin_function ("cgetreg32", BT_FN_LONG_CONST_PTR, 0, BUILT_IN_NORMAL, NULL,
			    NULL_TREE);


}


int plugin_init(struct plugin_name_args *plugin_info,
	struct plugin_gcc_version *version) {
const char *plugin_name = plugin_info->base_name;
struct register_pass_info pass_info;
struct register_pass_info pass_info2;
struct register_pass_info pass_info3;

/* To get context we need to include context.h */
pass_info.pass = make_pass_test_show_locus(g);
pass_info.reference_pass_name = "ssa";// // hook this pass with ssa builder pass
pass_info.ref_pass_instance_number = 1;// Insert the pass at the specified                                       instance number of the reference pass.                                       Do it for every instance if it is 0.
pass_info.pos_op = PASS_POS_INSERT_AFTER;// After SSA is built


/* To get context we need to include context.h */
pass_info2.pass = make_pass_safe_stack(g);
pass_info2.reference_pass_name = "vregs"; // hook this pass with ssa builder pass
pass_info2.ref_pass_instance_number = 1; // Insert the pass at the specified                                       instance number of the reference pass.                                       Do it for every instance if it is 0.
pass_info2.pos_op = PASS_POS_INSERT_AFTER; // After SSA is built

/* To get context we need to include context.h */
pass_info3.pass = make_pass_pro_epi_fixup(g);
pass_info3.reference_pass_name = "pro_and_epilogue"; // hook this pass with ssa builder pass
pass_info3.ref_pass_instance_number = 1; // Insert the pass at the specified                                       instance number of the reference pass.                                       Do it for every instance if it is 0.
pass_info3.pos_op = PASS_POS_INSERT_AFTER; // After SSA is built

/* 
register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info);
register_callback(plugin_name, PLUGIN_ALL_IPA_PASSES_START, plugin_func, NULL);
register_callback(plugin_name, PLUGIN_START_UNIT, initialize_builtins, NULL);
register_callback(plugin_name, PLUGIN_ATTRIBUTES, plugin_attr_func, NULL);
register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info2);
register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info3);
*/

/* Open database */
if (plugin_info -> argc > 0)
{
	char  func_names[200];
	string line;
	DEBUG("Opening Databse:%s \n",plugin_info->argv[0].value);
	// create and open a character archive for output
	std::ifstream ifs;
	memset(func_names,0,sizeof(func_names));

	//outfile.open(plugin_info->argv[0].value, ios::out | ios::trunc );
    ifs.open(plugin_info->argv[0].value, ios::out | ios::in );
    while (getline (ifs,line))
    {
    	memset(func_names,0,sizeof(func_names));
    	memcpy(func_names,line.c_str(), line.length());
    	DEBUG("Registering %s \n", func_names);
    	saveFunc(func_names);
    }
    ifs.close();
    ofs.open(plugin_info->argv[0].value, std::ios_base::app);
}

    std::ifstream ifs;
    ifs.open("/home/arslan/indirect_references", ios::out | ios::in );

    string line;
    if(getline(ifs,line))
    {
    	stringstream buffer1(line);
    	buffer1>>totalRef ;
    	getline(ifs,line);
    	stringstream buffer2(line);
    	buffer2>>instrRef;
			getline(ifs, line);
			stringstream buffer3( line);
			buffer3 >> instrAdded;
			getline(ifs, line);
			stringstream buffer4( line);
			buffer4 >> callsAdded;
    }
    else
    {
    	totalRef = 0;
    	instrRef = 0;
    }
    ifs.close();
return 0;
}
