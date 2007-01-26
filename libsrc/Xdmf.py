# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.31
#
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _Xdmf
import new
new_instancemethod = new.instancemethod
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


_FILE_OFFSET_BITS = _Xdmf._FILE_OFFSET_BITS
XDMF_SUCCESS = _Xdmf.XDMF_SUCCESS
XDMF_FAIL = _Xdmf.XDMF_FAIL
XDMF_TRUE = _Xdmf.XDMF_TRUE
XDMF_FALSE = _Xdmf.XDMF_FALSE
XDMF_MAX_DIMENSION = _Xdmf.XDMF_MAX_DIMENSION
XDMF_MAX_STRING_LENGTH = _Xdmf.XDMF_MAX_STRING_LENGTH
XDMF_DEFAULT_INDEX = _Xdmf.XDMF_DEFAULT_INDEX
XDMF_SELECT_SLAB = _Xdmf.XDMF_SELECT_SLAB
XDMF_SELECT_INDEX = _Xdmf.XDMF_SELECT_INDEX
XDMF_UNKNOWN_TYPE = _Xdmf.XDMF_UNKNOWN_TYPE
XDMF_INT8_TYPE = _Xdmf.XDMF_INT8_TYPE
XDMF_INT16_TYPE = _Xdmf.XDMF_INT16_TYPE
XDMF_INT32_TYPE = _Xdmf.XDMF_INT32_TYPE
XDMF_INT64_TYPE = _Xdmf.XDMF_INT64_TYPE
XDMF_FLOAT32_TYPE = _Xdmf.XDMF_FLOAT32_TYPE
XDMF_FLOAT64_TYPE = _Xdmf.XDMF_FLOAT64_TYPE
XDMF_UINT8_TYPE = _Xdmf.XDMF_UINT8_TYPE
XDMF_UINT16_TYPE = _Xdmf.XDMF_UINT16_TYPE
XDMF_UINT32_TYPE = _Xdmf.XDMF_UINT32_TYPE
XDMF_COMPOUND_TYPE = _Xdmf.XDMF_COMPOUND_TYPE
class XdmfObject(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfObject, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfObject, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfObject(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfObject
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfObject_GetClassName(*args)
    def SetDebug(*args): return _Xdmf.XdmfObject_SetDebug(*args)
    def GetDebug(*args): return _Xdmf.XdmfObject_GetDebug(*args)
    def GetGlobalDebug(*args): return _Xdmf.XdmfObject_GetGlobalDebug(*args)
    def SetGlobalDebug(*args): return _Xdmf.XdmfObject_SetGlobalDebug(*args)
    def DebugOn(*args): return _Xdmf.XdmfObject_DebugOn(*args)
    def DebugOff(*args): return _Xdmf.XdmfObject_DebugOff(*args)
    def GlobalDebugOn(*args): return _Xdmf.XdmfObject_GlobalDebugOn(*args)
    def GlobalDebugOff(*args): return _Xdmf.XdmfObject_GlobalDebugOff(*args)
    def GetUniqueName(*args): return _Xdmf.XdmfObject_GetUniqueName(*args)
XdmfObject_swigregister = _Xdmf.XdmfObject_swigregister
XdmfObject_swigregister(XdmfObject)

SetGlobalDebugOn = _Xdmf.SetGlobalDebugOn
SetGlobalDebugOff = _Xdmf.SetGlobalDebugOff
GetGlobalDebug = _Xdmf.GetGlobalDebug
SetGlobalDebug = _Xdmf.SetGlobalDebug
XdmfObjectToHandle = _Xdmf.XdmfObjectToHandle
HandleToXdmfObject = _Xdmf.HandleToXdmfObject
XdmfTypeToHDF5Type = _Xdmf.XdmfTypeToHDF5Type
HDF5TypeToXdmfType = _Xdmf.HDF5TypeToXdmfType
XdmfTypeToString = _Xdmf.XdmfTypeToString
StringToXdmfType = _Xdmf.StringToXdmfType
XdmfTypeToClassString = _Xdmf.XdmfTypeToClassString
XDMF_SELECTALL = _Xdmf.XDMF_SELECTALL
XDMF_HYPERSLAB = _Xdmf.XDMF_HYPERSLAB
XDMF_COORDINATES = _Xdmf.XDMF_COORDINATES
class XdmfDataDesc(XdmfObject):
    __swig_setmethods__ = {}
    for _s in [XdmfObject]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfDataDesc, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfObject]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfDataDesc, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfDataDesc(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfDataDesc
    __del__ = lambda self : None;
    def Print(*args): return _Xdmf.XdmfDataDesc_Print(*args)
    def SetNumberType(*args): return _Xdmf.XdmfDataDesc_SetNumberType(*args)
    def SetNumberTypeFromString(*args): return _Xdmf.XdmfDataDesc_SetNumberTypeFromString(*args)
    def GetNumberType(*args): return _Xdmf.XdmfDataDesc_GetNumberType(*args)
    def GetNumberTypeAsString(*args): return _Xdmf.XdmfDataDesc_GetNumberTypeAsString(*args)
    def SetShape(*args): return _Xdmf.XdmfDataDesc_SetShape(*args)
    def GetShape(*args): return _Xdmf.XdmfDataDesc_GetShape(*args)
    def GetShapeAsString(*args): return _Xdmf.XdmfDataDesc_GetShapeAsString(*args)
    def CopySelection(*args): return _Xdmf.XdmfDataDesc_CopySelection(*args)
    def CopyShape(*args): return _Xdmf.XdmfDataDesc_CopyShape(*args)
    def CopyType(*args): return _Xdmf.XdmfDataDesc_CopyType(*args)
    def SetNumberOfElements(*args): return _Xdmf.XdmfDataDesc_SetNumberOfElements(*args)
    def SetShapeFromString(*args): return _Xdmf.XdmfDataDesc_SetShapeFromString(*args)
    def SelectAll(*args): return _Xdmf.XdmfDataDesc_SelectAll(*args)
    def SelectHyperSlab(*args): return _Xdmf.XdmfDataDesc_SelectHyperSlab(*args)
    def SelectHyperSlabFromString(*args): return _Xdmf.XdmfDataDesc_SelectHyperSlabFromString(*args)
    def GetHyperSlab(*args): return _Xdmf.XdmfDataDesc_GetHyperSlab(*args)
    def GetHyperSlabAsString(*args): return _Xdmf.XdmfDataDesc_GetHyperSlabAsString(*args)
    def SelectCoordinates(*args): return _Xdmf.XdmfDataDesc_SelectCoordinates(*args)
    def SelectCoordinatesFromString(*args): return _Xdmf.XdmfDataDesc_SelectCoordinatesFromString(*args)
    def GetCoordinates(*args): return _Xdmf.XdmfDataDesc_GetCoordinates(*args)
    def GetCoordinatesAsString(*args): return _Xdmf.XdmfDataDesc_GetCoordinatesAsString(*args)
    def GetSelectionSize(*args): return _Xdmf.XdmfDataDesc_GetSelectionSize(*args)
    def GetNumberOfElements(*args): return _Xdmf.XdmfDataDesc_GetNumberOfElements(*args)
    def GetRank(*args): return _Xdmf.XdmfDataDesc_GetRank(*args)
    def GetDimension(*args): return _Xdmf.XdmfDataDesc_GetDimension(*args)
    def GetStart(*args): return _Xdmf.XdmfDataDesc_GetStart(*args)
    def GetStride(*args): return _Xdmf.XdmfDataDesc_GetStride(*args)
    def GetCount(*args): return _Xdmf.XdmfDataDesc_GetCount(*args)
    def GetSelectionType(*args): return _Xdmf.XdmfDataDesc_GetSelectionType(*args)
    def GetSelectionTypeAsString(*args): return _Xdmf.XdmfDataDesc_GetSelectionTypeAsString(*args)
    def GetDataType(*args): return _Xdmf.XdmfDataDesc_GetDataType(*args)
    def GetDataSpace(*args): return _Xdmf.XdmfDataDesc_GetDataSpace(*args)
    def GetElementSize(*args): return _Xdmf.XdmfDataDesc_GetElementSize(*args)
    def AddCompoundMemberFromString(*args): return _Xdmf.XdmfDataDesc_AddCompoundMemberFromString(*args)
    def GetNumberOfMembers(*args): return _Xdmf.XdmfDataDesc_GetNumberOfMembers(*args)
    def GetMemberName(*args): return _Xdmf.XdmfDataDesc_GetMemberName(*args)
    def GetMemberLength(*args): return _Xdmf.XdmfDataDesc_GetMemberLength(*args)
    def GetMemberSize(*args): return _Xdmf.XdmfDataDesc_GetMemberSize(*args)
    def GetMemberShape(*args): return _Xdmf.XdmfDataDesc_GetMemberShape(*args)
    def GetMemberShapeAsString(*args): return _Xdmf.XdmfDataDesc_GetMemberShapeAsString(*args)
    def GetMemberType(*args): return _Xdmf.XdmfDataDesc_GetMemberType(*args)
    def GetMemberTypeAsString(*args): return _Xdmf.XdmfDataDesc_GetMemberTypeAsString(*args)
    def GetMemberOffset(*args): return _Xdmf.XdmfDataDesc_GetMemberOffset(*args)
XdmfDataDesc_swigregister = _Xdmf.XdmfDataDesc_swigregister
XdmfDataDesc_swigregister(XdmfDataDesc)
GetUnique = _Xdmf.GetUnique

XDMF_ARRAY_TAG_LENGTH = _Xdmf.XDMF_ARRAY_TAG_LENGTH
class XdmfArray(XdmfDataDesc):
    __swig_setmethods__ = {}
    for _s in [XdmfDataDesc]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfArray, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfDataDesc]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfArray, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfArray(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfArray
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfArray_GetClassName(*args)
    def GetTagName(*args): return _Xdmf.XdmfArray_GetTagName(*args)
    def GetCoreLength(*args): return _Xdmf.XdmfArray_GetCoreLength(*args)
    def Allocate(*args): return _Xdmf.XdmfArray_Allocate(*args)
    def SetShape(*args): return _Xdmf.XdmfArray_SetShape(*args)
    def SetShapeFromString(*args): return _Xdmf.XdmfArray_SetShapeFromString(*args)
    def SetNumberOfElements(*args): return _Xdmf.XdmfArray_SetNumberOfElements(*args)
    def ReformFromString(*args): return _Xdmf.XdmfArray_ReformFromString(*args)
    def CopyShape(*args): return _Xdmf.XdmfArray_CopyShape(*args)
    def GetDataPointer(*args): return _Xdmf.XdmfArray_GetDataPointer(*args)
    def SetDataPointer(*args): return _Xdmf.XdmfArray_SetDataPointer(*args)
    def SetValueFromFloat64(*args): return _Xdmf.XdmfArray_SetValueFromFloat64(*args)
    def SetValueFromInt64(*args): return _Xdmf.XdmfArray_SetValueFromInt64(*args)
    def SetValues(*args): return _Xdmf.XdmfArray_SetValues(*args)
    def GetValuesAsInt8(*args): return _Xdmf.XdmfArray_GetValuesAsInt8(*args)
    def SetValuesFromInt8(*args): return _Xdmf.XdmfArray_SetValuesFromInt8(*args)
    def GetValuesAsInt32(*args): return _Xdmf.XdmfArray_GetValuesAsInt32(*args)
    def SetValuesFromInt32(*args): return _Xdmf.XdmfArray_SetValuesFromInt32(*args)
    def GetValuesAsInt64(*args): return _Xdmf.XdmfArray_GetValuesAsInt64(*args)
    def SetValuesFromInt64(*args): return _Xdmf.XdmfArray_SetValuesFromInt64(*args)
    def GetValuesAsFloat32(*args): return _Xdmf.XdmfArray_GetValuesAsFloat32(*args)
    def SetValuesFromFloat32(*args): return _Xdmf.XdmfArray_SetValuesFromFloat32(*args)
    def GetValuesAsFloat64(*args): return _Xdmf.XdmfArray_GetValuesAsFloat64(*args)
    def SetValuesFromFloat64(*args): return _Xdmf.XdmfArray_SetValuesFromFloat64(*args)
    def GetValueAsFloat64(*args): return _Xdmf.XdmfArray_GetValueAsFloat64(*args)
    def GetValueAsFloat32(*args): return _Xdmf.XdmfArray_GetValueAsFloat32(*args)
    def GetValueAsInt64(*args): return _Xdmf.XdmfArray_GetValueAsInt64(*args)
    def GetValueAsInt32(*args): return _Xdmf.XdmfArray_GetValueAsInt32(*args)
    def GetValueAsInt16(*args): return _Xdmf.XdmfArray_GetValueAsInt16(*args)
    def GetValueAsInt8(*args): return _Xdmf.XdmfArray_GetValueAsInt8(*args)
    def GetValues(*args): return _Xdmf.XdmfArray_GetValues(*args)
    def Generate(*args): return _Xdmf.XdmfArray_Generate(*args)
    def Clone(*args): return _Xdmf.XdmfArray_Clone(*args)
    def Reference(*args): return _Xdmf.XdmfArray_Reference(*args)
    def GetMaxAsFloat64(*args): return _Xdmf.XdmfArray_GetMaxAsFloat64(*args)
    def GetMinAsFloat64(*args): return _Xdmf.XdmfArray_GetMinAsFloat64(*args)
    def GetMaxAsInt64(*args): return _Xdmf.XdmfArray_GetMaxAsInt64(*args)
    def GetMinAsInt64(*args): return _Xdmf.XdmfArray_GetMinAsInt64(*args)
    def GetMean(*args): return _Xdmf.XdmfArray_GetMean(*args)
    def SetDataFromChars(*args): return _Xdmf.XdmfArray_SetDataFromChars(*args)
    def GetDataPointerAsCharPointer(*args): return _Xdmf.XdmfArray_GetDataPointerAsCharPointer(*args)
XdmfArray_swigregister = _Xdmf.XdmfArray_swigregister
XdmfArray_swigregister(XdmfArray)

class XdmfArrayList(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfArrayList, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfArrayList, name)
    __repr__ = _swig_repr
    __swig_setmethods__["name"] = _Xdmf.XdmfArrayList_name_set
    __swig_getmethods__["name"] = _Xdmf.XdmfArrayList_name_get
    if _newclass:name = _swig_property(_Xdmf.XdmfArrayList_name_get, _Xdmf.XdmfArrayList_name_set)
    __swig_setmethods__["timecntr"] = _Xdmf.XdmfArrayList_timecntr_set
    __swig_getmethods__["timecntr"] = _Xdmf.XdmfArrayList_timecntr_get
    if _newclass:timecntr = _swig_property(_Xdmf.XdmfArrayList_timecntr_get, _Xdmf.XdmfArrayList_timecntr_set)
    __swig_setmethods__["Array"] = _Xdmf.XdmfArrayList_Array_set
    __swig_getmethods__["Array"] = _Xdmf.XdmfArrayList_Array_get
    if _newclass:Array = _swig_property(_Xdmf.XdmfArrayList_Array_get, _Xdmf.XdmfArrayList_Array_set)
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfArrayList(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfArrayList
    __del__ = lambda self : None;
XdmfArrayList_swigregister = _Xdmf.XdmfArrayList_swigregister
XdmfArrayList_swigregister(XdmfArrayList)

TagNameToArray = _Xdmf.TagNameToArray
PrintAllXdmfArrays = _Xdmf.PrintAllXdmfArrays
GetCurrentArrayTime = _Xdmf.GetCurrentArrayTime
class XdmfLightData(XdmfObject):
    __swig_setmethods__ = {}
    for _s in [XdmfObject]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfLightData, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfObject]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfLightData, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfLightData(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfLightData
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfLightData_GetClassName(*args)
    def SetName(*args): return _Xdmf.XdmfLightData_SetName(*args)
    def GetName(*args): return _Xdmf.XdmfLightData_GetName(*args)
    def SetFileName(*args): return _Xdmf.XdmfLightData_SetFileName(*args)
    def GetFileName(*args): return _Xdmf.XdmfLightData_GetFileName(*args)
    def SetWorkingDirectory(*args): return _Xdmf.XdmfLightData_SetWorkingDirectory(*args)
    def GetWorkingDirectory(*args): return _Xdmf.XdmfLightData_GetWorkingDirectory(*args)
    def GetInitialized(*args): return _Xdmf.XdmfLightData_GetInitialized(*args)
    def SetInitialized(*args): return _Xdmf.XdmfLightData_SetInitialized(*args)
    def GetReturnBuffer(*args): return _Xdmf.XdmfLightData_GetReturnBuffer(*args)
XdmfLightData_swigregister = _Xdmf.XdmfLightData_swigregister
XdmfLightData_swigregister(XdmfLightData)
GetNextOlderArray = _Xdmf.GetNextOlderArray

class XdmfDOM(XdmfLightData):
    __swig_setmethods__ = {}
    for _s in [XdmfLightData]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfDOM, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfLightData]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfDOM, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfDOM(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfDOM
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfDOM_GetClassName(*args)
    def SetInputFileName(*args): return _Xdmf.XdmfDOM_SetInputFileName(*args)
    def SetOutputFileName(*args): return _Xdmf.XdmfDOM_SetOutputFileName(*args)
    def GetInputFileName(*args): return _Xdmf.XdmfDOM_GetInputFileName(*args)
    def GetOutputFileName(*args): return _Xdmf.XdmfDOM_GetOutputFileName(*args)
    def SetParserOptions(*args): return _Xdmf.XdmfDOM_SetParserOptions(*args)
    def GetOutput(*args): return _Xdmf.XdmfDOM_GetOutput(*args)
    def SetOutput(*args): return _Xdmf.XdmfDOM_SetOutput(*args)
    def GetInput(*args): return _Xdmf.XdmfDOM_GetInput(*args)
    def SetInput(*args): return _Xdmf.XdmfDOM_SetInput(*args)
    def GenerateHead(*args): return _Xdmf.XdmfDOM_GenerateHead(*args)
    def Puts(*args): return _Xdmf.XdmfDOM_Puts(*args)
    def GenerateTail(*args): return _Xdmf.XdmfDOM_GenerateTail(*args)
    def GetTree(*args): return _Xdmf.XdmfDOM_GetTree(*args)
    def __Parse(*args): return _Xdmf.XdmfDOM___Parse(*args)
    def Parse(*args): return _Xdmf.XdmfDOM_Parse(*args)
    def GetRoot(*args): return _Xdmf.XdmfDOM_GetRoot(*args)
    def GetNumberOfChildren(*args): return _Xdmf.XdmfDOM_GetNumberOfChildren(*args)
    def GetChild(*args): return _Xdmf.XdmfDOM_GetChild(*args)
    def GetNumberOfAttributes(*args): return _Xdmf.XdmfDOM_GetNumberOfAttributes(*args)
    def GetAttributeName(*args): return _Xdmf.XdmfDOM_GetAttributeName(*args)
    def IsChild(*args): return _Xdmf.XdmfDOM_IsChild(*args)
    def Serialize(*args): return _Xdmf.XdmfDOM_Serialize(*args)
    def Write(*args): return _Xdmf.XdmfDOM_Write(*args)
    def Insert(*args): return _Xdmf.XdmfDOM_Insert(*args)
    def InsertFromString(*args): return _Xdmf.XdmfDOM_InsertFromString(*args)
    def Create(*args): return _Xdmf.XdmfDOM_Create(*args)
    def InsertNew(*args): return _Xdmf.XdmfDOM_InsertNew(*args)
    def DeleteNode(*args): return _Xdmf.XdmfDOM_DeleteNode(*args)
    def FindElement(*args): return _Xdmf.XdmfDOM_FindElement(*args)
    def FindElementByAttribute(*args): return _Xdmf.XdmfDOM_FindElementByAttribute(*args)
    def FindElementByPath(*args): return _Xdmf.XdmfDOM_FindElementByPath(*args)
    def FindNumberOfElements(*args): return _Xdmf.XdmfDOM_FindNumberOfElements(*args)
    def FindNumberOfElementsByAttribute(*args): return _Xdmf.XdmfDOM_FindNumberOfElementsByAttribute(*args)
    def GetNdgmHost(*args): return _Xdmf.XdmfDOM_GetNdgmHost(*args)
    def SetNdgmHost(*args): return _Xdmf.XdmfDOM_SetNdgmHost(*args)
    def Get(*args): return _Xdmf.XdmfDOM_Get(*args)
    def GetAttribute(*args): return _Xdmf.XdmfDOM_GetAttribute(*args)
    def GetCData(*args): return _Xdmf.XdmfDOM_GetCData(*args)
    def Set(*args): return _Xdmf.XdmfDOM_Set(*args)
XdmfDOM_swigregister = _Xdmf.XdmfDOM_swigregister
XdmfDOM_swigregister(XdmfDOM)

HandleToXdmfDOM = _Xdmf.HandleToXdmfDOM
XDMF_ELEMENT_STATE_UNINITIALIZED = _Xdmf.XDMF_ELEMENT_STATE_UNINITIALIZED
XDMF_ELEMENT_STATE_LIGHT_PARSED = _Xdmf.XDMF_ELEMENT_STATE_LIGHT_PARSED
XDMF_ELEMENT_STATE_HEAVY_READ = _Xdmf.XDMF_ELEMENT_STATE_HEAVY_READ
class XdmfElement(XdmfLightData):
    __swig_setmethods__ = {}
    for _s in [XdmfLightData]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfElement, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfLightData]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfElement, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfElement(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfElement
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfElement_GetClassName(*args)
    def SetDOM(*args): return _Xdmf.XdmfElement_SetDOM(*args)
    def GetDOM(*args): return _Xdmf.XdmfElement_GetDOM(*args)
    def SetState(*args): return _Xdmf.XdmfElement_SetState(*args)
    def GetState(*args): return _Xdmf.XdmfElement_GetState(*args)
    def SetIsReference(*args): return _Xdmf.XdmfElement_SetIsReference(*args)
    def GetIsReference(*args): return _Xdmf.XdmfElement_GetIsReference(*args)
    def CheckForReference(*args): return _Xdmf.XdmfElement_CheckForReference(*args)
    def InsertChildElement(*args): return _Xdmf.XdmfElement_InsertChildElement(*args)
    def Serialize(*args): return _Xdmf.XdmfElement_Serialize(*args)
    def SetElement(*args): return _Xdmf.XdmfElement_SetElement(*args)
    def GetElement(*args): return _Xdmf.XdmfElement_GetElement(*args)
    def SetCopyReferenceData(*args): return _Xdmf.XdmfElement_SetCopyReferenceData(*args)
    def GetCopyReferenceData(*args): return _Xdmf.XdmfElement_GetCopyReferenceData(*args)
    def GetElementType(*args): return _Xdmf.XdmfElement_GetElementType(*args)
    def UpdateInformation(*args): return _Xdmf.XdmfElement_UpdateInformation(*args)
    def Update(*args): return _Xdmf.XdmfElement_Update(*args)
    def Build(*args): return _Xdmf.XdmfElement_Build(*args)
    def Set(*args): return _Xdmf.XdmfElement_Set(*args)
    def Get(*args): return _Xdmf.XdmfElement_Get(*args)
    def Copy(*args): return _Xdmf.XdmfElement_Copy(*args)
    def FollowReference(*args): return _Xdmf.XdmfElement_FollowReference(*args)
XdmfElement_swigregister = _Xdmf.XdmfElement_swigregister
XdmfElement_swigregister(XdmfElement)

class XdmfInformation(XdmfElement):
    __swig_setmethods__ = {}
    for _s in [XdmfElement]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfInformation, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfElement]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfInformation, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfInformation(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfInformation
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfInformation_GetClassName(*args)
    def UpdateInformation(*args): return _Xdmf.XdmfInformation_UpdateInformation(*args)
    def SetValue(*args): return _Xdmf.XdmfInformation_SetValue(*args)
    def Build(*args): return _Xdmf.XdmfInformation_Build(*args)
    def GetValue(*args): return _Xdmf.XdmfInformation_GetValue(*args)
XdmfInformation_swigregister = _Xdmf.XdmfInformation_swigregister
XdmfInformation_swigregister(XdmfInformation)

XDMF_FORMAT_XML = _Xdmf.XDMF_FORMAT_XML
XDMF_FORMAT_HDF = _Xdmf.XDMF_FORMAT_HDF
XDMF_ITEM_UNIFORM = _Xdmf.XDMF_ITEM_UNIFORM
XDMF_ITEM_HYPERSLAB = _Xdmf.XDMF_ITEM_HYPERSLAB
XDMF_ITEM_COORDINATES = _Xdmf.XDMF_ITEM_COORDINATES
XDMF_ITEM_FUNCTION = _Xdmf.XDMF_ITEM_FUNCTION
XDMF_ITEM_COLLECTION = _Xdmf.XDMF_ITEM_COLLECTION
XDMF_ITEM_TREE = _Xdmf.XDMF_ITEM_TREE
XDMF_ITEM_MASK = _Xdmf.XDMF_ITEM_MASK
class XdmfDataItem(XdmfElement):
    __swig_setmethods__ = {}
    for _s in [XdmfElement]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfDataItem, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfElement]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfDataItem, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfDataItem(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfDataItem
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfDataItem_GetClassName(*args)
    def GetValues(*args): return _Xdmf.XdmfDataItem_GetValues(*args)
    def GetFormat(*args): return _Xdmf.XdmfDataItem_GetFormat(*args)
    def SetFormat(*args): return _Xdmf.XdmfDataItem_SetFormat(*args)
    def SetArrayIsMine(*args): return _Xdmf.XdmfDataItem_SetArrayIsMine(*args)
    def GetArrayIsMine(*args): return _Xdmf.XdmfDataItem_GetArrayIsMine(*args)
    def UpdateInformation(*args): return _Xdmf.XdmfDataItem_UpdateInformation(*args)
    def Update(*args): return _Xdmf.XdmfDataItem_Update(*args)
    def Build(*args): return _Xdmf.XdmfDataItem_Build(*args)
    def GetDataDesc(*args): return _Xdmf.XdmfDataItem_GetDataDesc(*args)
    def SetDataDesc(*args): return _Xdmf.XdmfDataItem_SetDataDesc(*args)
    def GetArray(*args): return _Xdmf.XdmfDataItem_GetArray(*args)
    def SetArray(*args): return _Xdmf.XdmfDataItem_SetArray(*args)
    def GetDataValues(*args): return _Xdmf.XdmfDataItem_GetDataValues(*args)
    def SetDataValues(*args): return _Xdmf.XdmfDataItem_SetDataValues(*args)
    def GetRank(*args): return _Xdmf.XdmfDataItem_GetRank(*args)
    def SetShape(*args): return _Xdmf.XdmfDataItem_SetShape(*args)
    def GetShape(*args): return _Xdmf.XdmfDataItem_GetShape(*args)
    def GetShapeAsString(*args): return _Xdmf.XdmfDataItem_GetShapeAsString(*args)
    def GetDimensions(*args): return _Xdmf.XdmfDataItem_GetDimensions(*args)
    def SetDimensions(*args): return _Xdmf.XdmfDataItem_SetDimensions(*args)
    def SetDimensionsFromString(*args): return _Xdmf.XdmfDataItem_SetDimensionsFromString(*args)
    def SetHeavyDataSetName(*args): return _Xdmf.XdmfDataItem_SetHeavyDataSetName(*args)
    def GetHeavyDataSetName(*args): return _Xdmf.XdmfDataItem_GetHeavyDataSetName(*args)
    def Copy(*args): return _Xdmf.XdmfDataItem_Copy(*args)
    def SetItemType(*args): return _Xdmf.XdmfDataItem_SetItemType(*args)
    def GetItemType(*args): return _Xdmf.XdmfDataItem_GetItemType(*args)
    def GetIsMultiple(*args): return _Xdmf.XdmfDataItem_GetIsMultiple(*args)
    def SetFunction(*args): return _Xdmf.XdmfDataItem_SetFunction(*args)
    def GetFunction(*args): return _Xdmf.XdmfDataItem_GetFunction(*args)
XdmfDataItem_swigregister = _Xdmf.XdmfDataItem_swigregister
XdmfDataItem_swigregister(XdmfDataItem)

class XdmfDataStructure(XdmfDataItem):
    __swig_setmethods__ = {}
    for _s in [XdmfDataItem]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfDataStructure, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfDataItem]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfDataStructure, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfDataStructure(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfDataStructure
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfDataStructure_GetClassName(*args)
    def UpdateInformation(*args): return _Xdmf.XdmfDataStructure_UpdateInformation(*args)
XdmfDataStructure_swigregister = _Xdmf.XdmfDataStructure_swigregister
XdmfDataStructure_swigregister(XdmfDataStructure)

class XdmfValues(XdmfDataItem):
    __swig_setmethods__ = {}
    for _s in [XdmfDataItem]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfValues, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfDataItem]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfValues, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfValues(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfValues
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfValues_GetClassName(*args)
    def Inherit(*args): return _Xdmf.XdmfValues_Inherit(*args)
    def Read(*args): return _Xdmf.XdmfValues_Read(*args)
    def Write(*args): return _Xdmf.XdmfValues_Write(*args)
XdmfValues_swigregister = _Xdmf.XdmfValues_swigregister
XdmfValues_swigregister(XdmfValues)

class XdmfValuesXML(XdmfValues):
    __swig_setmethods__ = {}
    for _s in [XdmfValues]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfValuesXML, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfValues]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfValuesXML, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfValuesXML(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfValuesXML
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfValuesXML_GetClassName(*args)
    def Read(*args): return _Xdmf.XdmfValuesXML_Read(*args)
    def Write(*args): return _Xdmf.XdmfValuesXML_Write(*args)
XdmfValuesXML_swigregister = _Xdmf.XdmfValuesXML_swigregister
XdmfValuesXML_swigregister(XdmfValuesXML)

class XdmfValuesHDF(XdmfValues):
    __swig_setmethods__ = {}
    for _s in [XdmfValues]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfValuesHDF, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfValues]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfValuesHDF, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfValuesHDF(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfValuesHDF
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfValuesHDF_GetClassName(*args)
    def Read(*args): return _Xdmf.XdmfValuesHDF_Read(*args)
    def Write(*args): return _Xdmf.XdmfValuesHDF_Write(*args)
XdmfValuesHDF_swigregister = _Xdmf.XdmfValuesHDF_swigregister
XdmfValuesHDF_swigregister(XdmfValuesHDF)

XdmfArrayExpr = _Xdmf.XdmfArrayExpr
XdmfScalarExpr = _Xdmf.XdmfScalarExpr
XdmfExpr = _Xdmf.XdmfExpr
class XdmfHeavyData(XdmfDataDesc):
    __swig_setmethods__ = {}
    for _s in [XdmfDataDesc]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfHeavyData, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfDataDesc]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfHeavyData, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfHeavyData(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfHeavyData
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfHeavyData_GetClassName(*args)
    def GetNdgmHost(*args): return _Xdmf.XdmfHeavyData_GetNdgmHost(*args)
    def SetNdgmHost(*args): return _Xdmf.XdmfHeavyData_SetNdgmHost(*args)
    def GetWorkingDirectory(*args): return _Xdmf.XdmfHeavyData_GetWorkingDirectory(*args)
    def SetWorkingDirectory(*args): return _Xdmf.XdmfHeavyData_SetWorkingDirectory(*args)
    def GetDomain(*args): return _Xdmf.XdmfHeavyData_GetDomain(*args)
    def SetDomain(*args): return _Xdmf.XdmfHeavyData_SetDomain(*args)
    def GetFileName(*args): return _Xdmf.XdmfHeavyData_GetFileName(*args)
    def SetFileName(*args): return _Xdmf.XdmfHeavyData_SetFileName(*args)
    def GetPath(*args): return _Xdmf.XdmfHeavyData_GetPath(*args)
    def SetPath(*args): return _Xdmf.XdmfHeavyData_SetPath(*args)
    def GetAccess(*args): return _Xdmf.XdmfHeavyData_GetAccess(*args)
    def SetAccess(*args): return _Xdmf.XdmfHeavyData_SetAccess(*args)
XdmfHeavyData_swigregister = _Xdmf.XdmfHeavyData_swigregister
XdmfHeavyData_swigregister(XdmfHeavyData)

XDMF_H5_OTHER = _Xdmf.XDMF_H5_OTHER
class XdmfHDF(XdmfHeavyData):
    __swig_setmethods__ = {}
    for _s in [XdmfHeavyData]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, XdmfHDF, name, value)
    __swig_getmethods__ = {}
    for _s in [XdmfHeavyData]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, XdmfHDF, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _Xdmf.new_XdmfHDF(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _Xdmf.delete_XdmfHDF
    __del__ = lambda self : None;
    def GetClassName(*args): return _Xdmf.XdmfHDF_GetClassName(*args)
    def SetCompression(*args): return _Xdmf.XdmfHDF_SetCompression(*args)
    def GetCompression(*args): return _Xdmf.XdmfHDF_GetCompression(*args)
    def SetUseSerialFile(*args): return _Xdmf.XdmfHDF_SetUseSerialFile(*args)
    def GetUseSerialFile(*args): return _Xdmf.XdmfHDF_GetUseSerialFile(*args)
    def SetCwdName(*args): return _Xdmf.XdmfHDF_SetCwdName(*args)
    def GetCwdName(*args): return _Xdmf.XdmfHDF_GetCwdName(*args)
    def Cd(*args): return _Xdmf.XdmfHDF_Cd(*args)
    def Mkdir(*args): return _Xdmf.XdmfHDF_Mkdir(*args)
    def GetNumberOfChildren(*args): return _Xdmf.XdmfHDF_GetNumberOfChildren(*args)
    def GetHDFVersion(*args): return _Xdmf.XdmfHDF_GetHDFVersion(*args)
    def GetChild(*args): return _Xdmf.XdmfHDF_GetChild(*args)
    def SetNextChild(*args): return _Xdmf.XdmfHDF_SetNextChild(*args)
    def Info(*args): return _Xdmf.XdmfHDF_Info(*args)
    def GetChildType(*args): return _Xdmf.XdmfHDF_GetChildType(*args)
    def GetChildTypeAsString(*args): return _Xdmf.XdmfHDF_GetChildTypeAsString(*args)
    def CreateDataset(*args): return _Xdmf.XdmfHDF_CreateDataset(*args)
    def OpenDataset(*args): return _Xdmf.XdmfHDF_OpenDataset(*args)
    def Open(*args): return _Xdmf.XdmfHDF_Open(*args)
    def Read(*args): return _Xdmf.XdmfHDF_Read(*args)
    def Write(*args): return _Xdmf.XdmfHDF_Write(*args)
    def Close(*args): return _Xdmf.XdmfHDF_Close(*args)
XdmfHDF_swigregister = _Xdmf.XdmfHDF_swigregister
XdmfHDF_swigregister(XdmfHDF)


CopyArray = _Xdmf.CopyArray

