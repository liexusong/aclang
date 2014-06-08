/* see copyright notice in aclang.h */

#include "ac_variable.h"
#include "ac_gc.h"
#include "ac_vm.h"
#include "MurmurHash3.h"
#include <memory.h>
#include <sstream>

std::string getVarTypeStr(acVarType vt)
{
    switch(vt)
    {
    case acVT_NULL:
        return "null";
    case acVT_BOOL:
        return "bool";
    case acVT_INT32:
        return "int32";
    case acVT_INT64:
        return "int64";
    case acVT_FLOAT:
        return "float";
    case acVT_DOUBLE:
        return "double";
    case acVT_VAR:
        return "var";
    case acVT_STRING:
        return "string";
    case acVT_ARRAY:
        return "array";
    case acVT_TABLE:
        return "table";
    case acVT_FUNCTION:
        return "function";
    case acVT_DELEGATE:
        return "delegate";
    case acVT_USERDATA:
        return "userdata";
    case acVT_USERFUNC:
        return "userfunc";
    }
    return "unknwon type";
}

void acString::hash()
{
    MurmurHash3_x86_32(m_data.c_str(), m_data.length(), HASH_SEED, &m_hash, &m_sum);
}

void acVariable::assignFrom(acVariable* v)
{
    memcpy(this, v, sizeof(acVariable));
    m_gcColor = acGarbageCollector::GC_BLACK;//avoid to be gc
}

acVariable* acVariable::getBindFunc(char* name)
{
    if(m_bindFuncTable == 0) return 0;
    return m_bindFuncTable->get(name);
}

acVariable* acVariable::getBindFunc(acVariable* key)
{
    if(m_bindFuncTable == 0) return 0;
    return m_bindFuncTable->get(key);
}

void acVariable::setBindFunc(char* name, acVariable* func)
{
    if(!m_bftHasWritten)
    {
        //clone bindFuncTable

        m_bftHasWritten = true;
    }

}

void acVariable::setBindFunc(acVariable* key, acVariable* func)
{
    if(!m_bftHasWritten)
    {
        //clone bindFuncTable

        m_bftHasWritten = true;
    }
}

int acVariable::compare(acVariable* v, acVM* vm)
{
    switch(m_valueType)
    {
    case acVT_INT32:
        switch(v->m_valueType)
        {
        case acVT_INT32:
            return m_int32 - v->m_int32;
            break;
        case acVT_INT64:
            {
                acInt64 cmp = acInt64(m_int32) - v->m_int64;
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_FLOAT:
            {
                acFloat cmp = acFloat(m_int32) - v->m_float;
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_DOUBLE:
            {
                acDouble cmp = acDouble(m_int32) - v->m_double;
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        }
        break;
    case acVT_INT64:
        switch(v->m_valueType)
        {
        case acVT_INT32:
            {
                acInt64 cmp = m_int64 - acInt64(v->m_int32);
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_INT64:
            {
                acInt64 cmp = m_int64 - v->m_int64;
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_FLOAT:
            {
                acDouble cmp = acDouble(m_int64) - acDouble(v->m_float);
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_DOUBLE:
            {
                acDouble cmp = acDouble(m_int64) - v->m_double;
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        }
        break;
    case acVT_FLOAT:
        switch(v->m_valueType)
        {
        case acVT_INT32:
            {
                acFloat cmp = m_float - acFloat(v->m_int32);
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_INT64:
            {
                acDouble cmp = acDouble(m_float) - v->m_int64;
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_FLOAT:
            {
                acFloat cmp = m_float - v->m_float;
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_DOUBLE:
            {
                acDouble cmp = acDouble(m_float) - v->m_double;
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        }
        break;
    case acVT_DOUBLE:
        switch(v->m_valueType)
        {
        case acVT_INT32:
            {
                acDouble cmp = m_double - acDouble(v->m_int32);
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_INT64:
            {
                acDouble cmp = m_double - acDouble(v->m_int64);
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_FLOAT:
            {
                acFloat cmp = m_double - acDouble(v->m_float);
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        case acVT_DOUBLE:
            {
                acDouble cmp = m_double - v->m_double;
                return (cmp > 0)? 1 : ((cmp < 0)? -1 : 0);
            }
            break;
        }
        break;
    case acVT_STRING:
        if(v->m_valueType == acVT_STRING)
        {
            return ((acString*)m_gcobj)->m_data.compare(((acString*)v->m_gcobj)->m_data);
        }
        break;
    }

    vm->runtimeError(std::string("ERROR: comparsion between '")+getVarTypeStr(m_valueType)+"' and '"+getVarTypeStr(v->m_valueType)+"'");
    return 0;
}

acHashValue& acVariable::getHash()
{
    switch(m_valueType)
    {
    case acVT_BOOL:
    case acVT_INT32:
    case acVT_INT64:
    case acVT_FLOAT:
    case acVT_DOUBLE:
        m_hash.u64_0 = (acUInt64)m_int64;
        m_hash.u32_2 = 0;
        break;
    case acVT_STRING:
        m_hash.u32_0 = ((acString*)m_gcobj)->m_hash;
        m_hash.u32_1 = ((acString*)m_gcobj)->m_sum;
        m_hash.u32_2 = ((acString*)m_gcobj)->m_data.length();
        break;
    case acVT_VAR:
    case acVT_ARRAY:
    case acVT_TABLE:
    case acVT_FUNCTION:
    case acVT_DELEGATE:
    //user data
    case acVT_USERDATA:
    case acVT_USERFUNC:
        m_hash.u64_0 = (acUInt64)m_gcobj;
        m_hash.u32_2 = 0;
        break;
    }

    m_hash.u32_3 = m_valueType;

    return m_hash;
}

void acVariable::getHash(int value, acHashValue& hash)
{
    hash.u64_0 = (acUInt64)value;
    hash.u32_2 = 0;
    hash.u32_3 = acVT_INT32;
}

void acVariable::getHash(char* str, acHashValue& hash)
{
    acUInt32 h, s;
    acUInt32 len = strlen(str);
    MurmurHash3_x86_32(str, len, HASH_SEED, &h, &s);

    hash.u32_0 = h;
    hash.u32_1 = s;
    hash.u32_2 = len;
    hash.u32_3 = acVT_STRING;
}

//======================================
template <class T> 
std::string convertToString(T value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

template <class T> 
bool convertFromString(T& value, const std::string& str)
{
    std::stringstream ss(str);
    ss >> value;
    return ss.eof();
}

//======================================
//null base functions
bool null_toBool(acVariable* var, acVM* vm)
{
    return false;
}
acInt32 null_toInt32(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'null' to 'int32'");
    return 0;
}
acInt64 null_toInt64(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'null' to 'int64'");
    return 0;
}
acFloat null_toFloat(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'null' to 'float'");
    return 0;
}
acDouble null_toDouble(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'null' to 'double'");
    return 0;
}
std::string null_toStr(acVariable* var, acVM* vm)
{
    //vm->runtimeError("Error: attempt to convert 'null' to 'string'");
    return "null";
}
void null_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    vm->runtimeError("Error: attempt to call 'null'");
}

//======================================
//boolean base functions
bool bool_toBool(acVariable* var, acVM* vm)
{
    return var->m_int32 != 0;
}
acInt32 bool_toInt32(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'bool' to 'int32'");
    return 0;
}
acInt64 bool_toInt64(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'bool' to 'int64'");
    return 0;
}
acFloat bool_toFloat(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'bool' to 'float'");
    return 0;
}
acDouble bool_toDouble(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'bool' to 'double'");
    return 0;
}
std::string bool_toStr(acVariable* var, acVM* vm)
{
    //vm->runtimeError("Error: attempt to convert 'bool' to 'string'");
    return var->m_int32 != 0 ? "true" : "false";
}
void bool_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    vm->runtimeError("Error: attempt to call 'bool'");
}

//======================================
//int32 base functions
bool int32_toBool(acVariable* var, acVM* vm)
{
    return var->m_int32 != 0;
}
acInt32 int32_toInt32(acVariable* var, acVM* vm)
{
    return (acInt32)var->m_int32;
}
acInt64 int32_toInt64(acVariable* var, acVM* vm)
{
    return (acInt64)var->m_int32;
}
acFloat int32_toFloat(acVariable* var, acVM* vm)
{
    return (acFloat)var->m_int32;
}
acDouble int32_toDouble(acVariable* var, acVM* vm)
{
    return (acDouble)var->m_int32;
}
std::string int32_toStr(acVariable* var, acVM* vm)
{
    return convertToString(var->m_int32);
}
void int32_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    vm->runtimeError("Error: attempt to call 'int32'");
}

//======================================
//int64 base functions
bool int64_toBool(acVariable* var, acVM* vm)
{
    return var->m_int64 != 0;
}
acInt32 int64_toInt32(acVariable* var, acVM* vm)
{
    return (acInt32)var->m_int64;
}
acInt64 int64_toInt64(acVariable* var, acVM* vm)
{
    return (acInt64)var->m_int64;
}
acFloat int64_toFloat(acVariable* var, acVM* vm)
{
    return (acFloat)var->m_int64;
}
acDouble int64_toDouble(acVariable* var, acVM* vm)
{
    return (acDouble)var->m_int64;
}
std::string int64_toStr(acVariable* var, acVM* vm)
{
    return convertToString(var->m_int64)+"L";
}
void int64_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    vm->runtimeError("Error: attempt to call 'int64'");
}

//======================================
//float base functions
bool float_toBool(acVariable* var, acVM* vm)
{
    return var->m_float != 0;
}
acInt32 float_toInt32(acVariable* var, acVM* vm)
{
    return (acInt32)var->m_float;
}
acInt64 float_toInt64(acVariable* var, acVM* vm)
{
    return (acInt64)var->m_float;
}
acFloat float_toFloat(acVariable* var, acVM* vm)
{
    return (acFloat)var->m_float;
}
acDouble float_toDouble(acVariable* var, acVM* vm)
{
    return (acDouble)var->m_float;
}
std::string float_toStr(acVariable* var, acVM* vm)
{
    std::stringstream ss;
    //ss.precision(1);
    ss <<  var->m_float << "f";
    return ss.str();
}
void float_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    vm->runtimeError("Error: attempt to call 'float'");
}

//======================================
//double base functions
bool double_toBool(acVariable* var, acVM* vm)
{
    return var->m_double != 0;
}
acInt32 double_toInt32(acVariable* var, acVM* vm)
{
    return (acInt32)var->m_double;
}
acInt64 double_toInt64(acVariable* var, acVM* vm)
{
    return (acInt64)var->m_double;
}
acFloat double_toFloat(acVariable* var, acVM* vm)
{
    return (acFloat)var->m_double;
}
acDouble double_toDouble(acVariable* var, acVM* vm)
{
    return (acDouble)var->m_double;
}
std::string double_toStr(acVariable* var, acVM* vm)
{
    std::stringstream ss;
    //ss.precision(1);
    ss << std::fixed << var->m_double;
    return ss.str();
}
void double_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    vm->runtimeError("Error: attempt to call 'double'");
}

//======================================
//string base functions
bool str_toBool(acVariable* var, acVM* vm)
{
    return true;
}
acInt32 str_toInt32(acVariable* var, acVM* vm)
{
    acInt32 v;
    if(!convertFromString(v, ((acString*)var->m_gcobj)->m_data))
    {
        vm->runtimeError(std::string("Error: attempt to convert 'string' (")+((acString*)var->m_gcobj)->m_data+") to 'int32'");
    }
    return v;
}
acInt64 str_toInt64(acVariable* var, acVM* vm)
{
    acInt64 v;
    if(!convertFromString(v, ((acString*)var->m_gcobj)->m_data))
    {
        vm->runtimeError(std::string("Error: attempt to convert 'string' (")+((acString*)var->m_gcobj)->m_data+") to 'int64'");
    }
    return v;
}
acFloat str_toFloat(acVariable* var, acVM* vm)
{
    acFloat v;
    if(!convertFromString(v, ((acString*)var->m_gcobj)->m_data))
    {
        vm->runtimeError(std::string("Error: attempt to convert 'string' (")+((acString*)var->m_gcobj)->m_data+") to 'float'");
    }
    return v;
}
acDouble str_toDouble(acVariable* var, acVM* vm)
{
    acDouble v;
    if(!convertFromString(v, ((acString*)var->m_gcobj)->m_data))
    {
        vm->runtimeError(std::string("Error: attempt to convert 'string' (")+((acString*)var->m_gcobj)->m_data+") to 'double'");
    }
    return v;
}
std::string str_toStr(acVariable* var, acVM* vm)
{
    return ((acString*)var->m_gcobj)->m_data;
}
void str_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    vm->runtimeError("Error: attempt to call 'string'");
}

//======================================
//array base functions
bool arr_toBool(acVariable* var, acVM* vm)
{
    return true;
}
acInt32 arr_toInt32(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'array' to 'int32'");
    return 0;
}
acInt64 arr_toInt64(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'array' to 'int64'");
    return 0;
}
acFloat arr_toFloat(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'array' to 'float'");
    return 0;
}
acDouble arr_toDouble(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'array' to 'double'");
    return 0;
}
std::string arr_toStr(acVariable* var, acVM* vm)
{
    char buf[32];
    sprintf(buf, "array: 0x%08x", (acUInt32)var->m_gcobj);
    return std::string(buf);
}
void arr_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    vm->runtimeError("Error: attempt to call 'array'");
}

//======================================
//table base functions
bool tab_toBool(acVariable* var, acVM* vm)
{
    return true;
}
acInt32 tab_toInt32(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'table' to 'int32'");
    return 0;
}
acInt64 tab_toInt64(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'table' to 'int64'");
    return 0;
}
acFloat tab_toFloat(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'table' to 'float'");
    return 0;
}
acDouble tab_toDouble(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'table' to 'double'");
    return 0;
}
std::string tab_toStr(acVariable* var, acVM* vm)
{
    char buf[32];
    sprintf(buf, "table: 0x%08x", (acUInt32)var->m_gcobj);
    return std::string(buf);
}
void tab_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    vm->runtimeError("Error: attempt to call 'table'");
}

//======================================
//function base functions
bool func_toBool(acVariable* var, acVM* vm)
{
    return true;
}
acInt32 func_toInt32(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'function' to 'int32'");
    return 0;
}
acInt64 func_toInt64(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'function' to 'int64'");
    return 0;
}
acFloat func_toFloat(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'function' to 'float'");
    return 0;
}
acDouble func_toDouble(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'function' to 'double'");
    return 0;
}
std::string func_toStr(acVariable* var, acVM* vm)
{
    char buf[32];
    sprintf(buf, "function: 0x%08x", (acUInt32)var->m_gcobj);
    return std::string(buf);
}
void func_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    ExecutionEngine* ee = vm->getExecutionEngine();
    acGarbageCollector* gc = vm->getGarbageCollector();

    acFunction* func = (acFunction*)var->m_gcobj;

    typedef void (*PFN)(void*, void*, void*);
    PFN pfn = reinterpret_cast<PFN>(func->m_funcPtr);
    pfn(thisVar, argArray, func->m_upValueTable);
}

//======================================
//delegate base functions
bool dele_toBool(acVariable* var, acVM* vm)
{
    return true;
}
acInt32 dele_toInt32(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'delegate' to 'int32'");
    return 0;
}
acInt64 dele_toInt64(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'delegate' to 'int64'");
    return 0;
}
acFloat dele_toFloat(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'delegate' to 'float'");
    return 0;
}
acDouble dele_toDouble(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'delegate' to 'double'");
    return 0;
}
std::string dele_toStr(acVariable* var, acVM* vm)
{
    char buf[32];
    sprintf(buf, "delegate: 0x%08x", (acUInt32)var->m_gcobj);
    return std::string(buf);
}
void dele_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    ExecutionEngine* ee = vm->getExecutionEngine();
    acGarbageCollector* gc = vm->getGarbageCollector();

    acDelegate* dele = (acDelegate*)var->m_gcobj;
    acFunction* func = (acFunction*)dele->m_funcVar->m_gcobj;

    typedef void (*PFN)(void*, void*, void*);
    PFN pfn = reinterpret_cast<PFN>(func->m_funcPtr);
    pfn(thisVar, argArray, func->m_upValueTable);
}

//======================================
//userfunc base functions
bool uf_toBool(acVariable* var, acVM* vm)
{
    return true;
}
acInt32 uf_toInt32(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'userfunc' to 'int32'");
    return 0;
}
acInt64 uf_toInt64(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'userfunc' to 'int64'");
    return 0;
}
acFloat uf_toFloat(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'userfunc' to 'float'");
    return 0;
}
acDouble uf_toDouble(acVariable* var, acVM* vm)
{
    vm->runtimeError("Error: attempt to convert 'userfunc' to 'double'");
    return 0;
}
std::string uf_toStr(acVariable* var, acVM* vm)
{
    char buf[32];
    sprintf(buf, "userfunc: 0x%08x", (acUInt32)var->m_gcobj);
    return std::string(buf);
}
void uf_funcCall(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    ExecutionEngine* ee = vm->getExecutionEngine();
    acGarbageCollector* gc = vm->getGarbageCollector();

    //ret var = null
    vm->setUserFuncRetVar((acVariable*)gc->createObject(acVT_NULL));

    acUserFunc* uf = (acUserFunc*)var->m_gcobj;

    typedef void (*PFN)(void*, void*, void*);
    PFN pfn = reinterpret_cast<PFN>(uf->m_funcPtr);
    pfn(thisVar, argArray, vm);

    //set ret var
    if(argArray->size() > 0)
    {
        argArray->set(0, vm->getUserFuncRetVar());
    }
    else
    {
        argArray->add(vm->getUserFuncRetVar());
    }
}

//======================================
static void* null_baseFuncPtrs[acBF_MAX] =
{
    (void*)null_toBool,
    (void*)null_toInt32,
    (void*)null_toInt64,
    (void*)null_toFloat,
    (void*)null_toDouble,
    (void*)null_toStr,
    (void*)null_funcCall
};
static void* bool_baseFuncPtrs[acBF_MAX] =
{
    (void*)bool_toBool,
    (void*)bool_toInt32,
    (void*)bool_toInt64,
    (void*)bool_toFloat,
    (void*)bool_toDouble,
    (void*)bool_toStr,
    (void*)bool_funcCall
};
static void* int32_baseFuncPtrs[acBF_MAX] =
{
    (void*)int32_toBool,
    (void*)int32_toInt32,
    (void*)int32_toInt64,
    (void*)int32_toFloat,
    (void*)int32_toDouble,
    (void*)int32_toStr,
    (void*)int32_funcCall
};
static void* int64_baseFuncPtrs[acBF_MAX] =
{
    (void*)int64_toBool,
    (void*)int64_toInt32,
    (void*)int64_toInt64,
    (void*)int64_toFloat,
    (void*)int64_toDouble,
    (void*)int64_toStr,
    (void*)int64_funcCall
};
static void* float_baseFuncPtrs[acBF_MAX] =
{
    (void*)float_toBool,
    (void*)float_toInt32,
    (void*)float_toInt64,
    (void*)float_toFloat,
    (void*)float_toDouble,
    (void*)float_toStr,
    (void*)float_funcCall
};
static void* double_baseFuncPtrs[acBF_MAX] =
{
    (void*)double_toBool,
    (void*)double_toInt32,
    (void*)double_toInt64,
    (void*)double_toFloat,
    (void*)double_toDouble,
    (void*)double_toStr,
    (void*)double_funcCall
};
static void* str_baseFuncPtrs[acBF_MAX] =
{
    (void*)str_toBool,
    (void*)str_toInt32,
    (void*)str_toInt64,
    (void*)str_toFloat,
    (void*)str_toDouble,
    (void*)str_toStr,
    (void*)str_funcCall
};
static void* arr_baseFuncPtrs[acBF_MAX] =
{
    (void*)arr_toBool,
    (void*)arr_toInt32,
    (void*)arr_toInt64,
    (void*)arr_toFloat,
    (void*)arr_toDouble,
    (void*)arr_toStr,
    (void*)arr_funcCall
};
static void* tab_baseFuncPtrs[acBF_MAX] =
{
    (void*)tab_toBool,
    (void*)tab_toInt32,
    (void*)tab_toInt64,
    (void*)tab_toFloat,
    (void*)tab_toDouble,
    (void*)tab_toStr,
    (void*)tab_funcCall
};
static void* func_baseFuncPtrs[acBF_MAX] =
{
    (void*)func_toBool,
    (void*)func_toInt32,
    (void*)func_toInt64,
    (void*)func_toFloat,
    (void*)func_toDouble,
    (void*)func_toStr,
    (void*)func_funcCall
};
static void* dele_baseFuncPtrs[acBF_MAX] =
{
    (void*)dele_toBool,
    (void*)dele_toInt32,
    (void*)dele_toInt64,
    (void*)dele_toFloat,
    (void*)dele_toDouble,
    (void*)dele_toStr,
    (void*)dele_funcCall
};
static void* uf_baseFuncPtrs[acBF_MAX] =
{
    (void*)uf_toBool,
    (void*)uf_toInt32,
    (void*)uf_toInt64,
    (void*)uf_toFloat,
    (void*)uf_toDouble,
    (void*)uf_toStr,
    (void*)uf_funcCall
};
//======================================
void acVariable::setBaseFuncPtrs(acVariable* var)
{
    switch(var->m_valueType)
    {
    case acVT_NULL:
        var->m_baseFuncPtrs = null_baseFuncPtrs;
        break;
    case acVT_BOOL:
        var->m_baseFuncPtrs = bool_baseFuncPtrs;
        break;
    case acVT_INT32:
        var->m_baseFuncPtrs = int32_baseFuncPtrs;
        break;
    case acVT_INT64:
        var->m_baseFuncPtrs = int64_baseFuncPtrs;
        break;
    case acVT_FLOAT:
        var->m_baseFuncPtrs = float_baseFuncPtrs;
        break;
    case acVT_DOUBLE:
        var->m_baseFuncPtrs = double_baseFuncPtrs;
        break;
    case acVT_STRING:
        var->m_baseFuncPtrs = str_baseFuncPtrs;
        break;
    case acVT_ARRAY:
        var->m_baseFuncPtrs = arr_baseFuncPtrs;
        break;
    case acVT_TABLE:
        var->m_baseFuncPtrs = tab_baseFuncPtrs;
        break;
    case acVT_FUNCTION:
        var->m_baseFuncPtrs = func_baseFuncPtrs;
        break;
    case acVT_DELEGATE:
        var->m_baseFuncPtrs = dele_baseFuncPtrs;
        break;
    case acVT_USERFUNC:
        var->m_baseFuncPtrs = uf_baseFuncPtrs;
        break;
    default:
        break;
    }
}

//======================================
//for codegen
bool toBool(acVariable* var, acVM* vm)
{
    void* fp = var->m_baseFuncPtrs[acBF_TOBOOL];
    return ((bool(*)(acVariable*, acVM*))fp)(var, vm);
}
acInt32 toInt32(acVariable* var, acVM* vm)
{
    void* fp = var->m_baseFuncPtrs[acBF_TOINT32];
    return ((acInt32(*)(acVariable*, acVM*))fp)(var, vm);
}
acInt64 toInt64(acVariable* var, acVM* vm)
{
    void* fp = var->m_baseFuncPtrs[acBF_TOINT64];
    return ((acInt64(*)(acVariable*, acVM*))fp)(var, vm);
}
acFloat toFloat(acVariable* var, acVM* vm)
{
    void* fp = var->m_baseFuncPtrs[acBF_TOFLOAT];
    return ((acFloat(*)(acVariable*, acVM*))fp)(var, vm);
}
acDouble toDouble(acVariable* var, acVM* vm)
{
    void* fp = var->m_baseFuncPtrs[acBF_TODOUBLE];
    return ((acDouble(*)(acVariable*, acVM*))fp)(var, vm);
}
std::string toString(acVariable* var, acVM* vm)
{
    void* fp = var->m_baseFuncPtrs[acBF_TOSTR];
    return ((std::string(*)(acVariable*, acVM*))fp)(var, vm);
}
void callFunction(acVariable* var, acVariable* thisVar, acArray* argArray, acVM* vm)
{
    void* fp = var->m_baseFuncPtrs[acBF_CALL];
    return ((void(*)(acVariable*, acVariable*, acArray*, acVM*))fp)(var, thisVar, argArray, vm);
}

//======================================
void addInt32(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acInt32 a = toInt32(v1, vm);
    acInt32 b = toInt32(v2, vm);
    ret->setValue(a + b);
}

void subInt32(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acInt32 a = toInt32(v1, vm);
    acInt32 b = toInt32(v2, vm);
    ret->setValue(a - b);
}

void mulInt32(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acInt32 a = toInt32(v1, vm);
    acInt32 b = toInt32(v2, vm);
    ret->setValue(a * b);
}

void divInt32(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acInt32 a = toInt32(v1, vm);
    acInt32 b = toInt32(v2, vm);
    ret->setValue(a / b);
}

void modInt32(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acInt32 a = toInt32(v1, vm);
    acInt32 b = toInt32(v2, vm);
    ret->setValue(a % b);
}

void addInt64(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acInt64 a = toInt64(v1, vm);
    acInt64 b = toInt64(v2, vm);
    ret->setValue(a + b);
}

void subInt64(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acInt64 a = toInt64(v1, vm);
    acInt64 b = toInt64(v2, vm);
    ret->setValue(a - b);
}

void mulInt64(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acInt64 a = toInt64(v1, vm);
    acInt64 b = toInt64(v2, vm);
    ret->setValue(a * b);
}

void divInt64(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acInt64 a = toInt64(v1, vm);
    acInt64 b = toInt64(v2, vm);
    ret->setValue(a / b);
}

void modInt64(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acInt64 a = toInt64(v1, vm);
    acInt64 b = toInt64(v2, vm);
    ret->setValue(a % b);
}

void addFloat(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acFloat a = toFloat(v1, vm);
    acFloat b = toFloat(v2, vm);
    ret->setValue(a + b);
}

void subFloat(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acFloat a = toFloat(v1, vm);
    acFloat b = toFloat(v2, vm);
    ret->setValue(a - b);
}

void mulFloat(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acFloat a = toFloat(v1, vm);
    acFloat b = toFloat(v2, vm);
    ret->setValue(a * b);
}

void divFloat(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acFloat a = toFloat(v1, vm);
    acFloat b = toFloat(v2, vm);
    ret->setValue(a / b);
}

void modFloat(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acFloat a = toFloat(v1, vm);
    acFloat b = toFloat(v2, vm);
    ret->setValue(fmod(a, b));
}

void addDouble(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acDouble a = toDouble(v1, vm);
    acDouble b = toDouble(v2, vm);
    ret->setValue(a + b);
}

void subDouble(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acDouble a = toDouble(v1, vm);
    acDouble b = toDouble(v2, vm);
    ret->setValue(a - b);
}

void mulDouble(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acDouble a = toDouble(v1, vm);
    acDouble b = toDouble(v2, vm);
    ret->setValue(a * b);
}

void divDouble(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acDouble a = toDouble(v1, vm);
    acDouble b = toDouble(v2, vm);
    ret->setValue(a / b);
}

void modDouble(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    acDouble a = toDouble(v1, vm);
    acDouble b = toDouble(v2, vm);
    ret->setValue(fmod(a, b));
}

void addString(acVariable* ret, acVariable* v1, acVariable* v2, acVM* vm)
{
    std::string a = toString(v1, vm);
    std::string b = toString(v2, vm);
    acString* s = (acString*)vm->getGarbageCollector()->createObject(acVT_STRING);
    s->setData(a + b);
    ret->setValue(s);
}