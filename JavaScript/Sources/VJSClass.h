/*
* This file is part of Wakanda software, licensed by 4D under
*  (i) the GNU General Public License version 3 (GNU GPL v3), or
*  (ii) the Affero General Public License version 3 (AGPL v3) or
*  (iii) a commercial license.
* This file remains the exclusive property of 4D and/or its licensors
* and is protected by national and international legislations.
* In any event, Licensee's compliance with the terms and conditions
* of the applicable license constitutes a prerequisite to any use of this file.
* Except as otherwise expressly stated in the applicable license,
* such license does not include any other license or rights on this file,
* 4D's and/or its licensors' trademarks and/or other proprietary rights.
* Consequently, no title, copyright or other proprietary rights
* other than those specified in the applicable license is granted.
*/
#ifndef __VJSClass__
#define __VJSClass__

#include "JS4D.h"
#include "VJSValue.h"

BEGIN_TOOLBOX_NAMESPACE

#if !VERSIONWIN
	#define __cdecl
#endif

class XTOOLBOX_API VJSParms_withContext
{	
public:
	VJSParms_withContext( JS4D::ContextRef inContext, JS4D::ObjectRef inObject)
		: fObject( inContext, inObject)	{}
			
	~VJSParms_withContext();

			const VJSContext&				GetContext() const					{ return fObject.GetContext();}

			JS4D::ContextRef				GetContextRef() const				{ return fObject.GetContextRef();}

			void							ProtectValue( JS4D::ValueRef inValue) const;
			void							UnprotectValue( JS4D::ValueRef inValue) const;

			const VJSObject&				GetObject() const					{ return fObject;}

			UniChar							GetWildChar() const					{ return GetContext().GetWildChar(); };
			
protected:
			VJSObject						fObject;
};


//======================================================

class XTOOLBOX_API VJSParms_withException : public VJSParms_withContext
{
public:
	VJSParms_withException( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::ValueRef* outException)
		: VJSParms_withContext( inContext, inObject)
		, fException( outException)
		{
			fTaskContext = VTask::GetCurrent()->GetErrorContext( true);
			fErrorContext = fTaskContext->PushNewContext( false /* do not keep */, true /* silent */);
		}
		
	~VJSParms_withException()
		{
			JS4D::ConvertErrorContextToException( GetContextRef(), fErrorContext, fException);
			xbox_assert( fErrorContext == fTaskContext->GetLastContext());
			fTaskContext->PopContext();
			fTaskContext->RecycleContext( fErrorContext);
		}

			void							EvaluateScript( const XBOX::VString& inScript, XBOX::VValueSingle** outResult);

			JS4D::ExceptionRef*				GetExceptionRefPointer()	{ return fException;}
			void							SetException(JS4D::ExceptionRef inException) { *fException = inException; }
			
protected:
			JS4D::ExceptionRef*				fException;
			VErrorTaskContext*				fTaskContext;
			VErrorContext*					fErrorContext;
};


//======================================================

class XTOOLBOX_API VJSParms_withReturnValue : public virtual VJSParms_withException
{
public:
	VJSParms_withReturnValue( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::ValueRef* outException)
		: VJSParms_withException( inContext, inObject, outException)
		, fReturnValue( inContext)
		{;}
	
			template<class Type>
			void				ReturnNumber( const Type& inValue)					{ fReturnValue.SetNumber( inValue, fException);}
			void				ReturnBool( bool inValue)							{ fReturnValue.SetBool( inValue, fException);}
			void				ReturnString( const XBOX::VString& inString)		{ fReturnValue.SetString( inString, fException);}
			void				ReturnTime( const XBOX::VTime& inTime)				{ fReturnValue.SetTime( inTime, fException);}
			void				ReturnDuration( const XBOX::VDuration& inDuration)	{ fReturnValue.SetDuration( inDuration, fException);}
			void				ReturnVValue( const XBOX::VValueSingle& inValue)	{ fReturnValue.SetVValue( inValue, fException);}
			void				ReturnFile( XBOX::VFile *inFile)					{ fReturnValue.SetFile( inFile, fException);}
			void				ReturnFolder( XBOX::VFolder *inFolder)				{ fReturnValue.SetFolder( inFolder, fException);}
			void				ReturnFilePathAsFileOrFolder( const XBOX::VFilePath& inPath)	{ fReturnValue.SetFilePathAsFileOrFolder( inPath, fException);}
				
			void				ReturnValue( const VJSValue& inValue)				{ fReturnValue = inValue; }
			void				ReturnJSONValue( const VJSONValue& inValue)			{ fReturnValue.SetJSONValue( inValue, fException);}

			void				ReturnNullValue()									{ fReturnValue.SetNull(); }
			void				ReturnUndefinedValue()								{ fReturnValue.SetUndefined(); }

			const VJSValue&		GetReturnValue() const								{ return fReturnValue;}

protected:
			VJSValue			fReturnValue;
};

//======================================================


class XTOOLBOX_API VJSParms_initialize : public VJSParms_withContext
{
public:
	VJSParms_initialize( JS4D::ContextRef inContext, JS4D::ObjectRef inObject)
		: VJSParms_withContext( inContext, inObject)	{;}
	
private:
};


//======================================================


class XTOOLBOX_API VJSParms_finalize
{
public:
	VJSParms_finalize( JS4D::ObjectRef inObject)
		: fObject( inObject)	{;}

/*
			// UnprotectValue() crashes because there is no context. 
			// JSValueUnprotect(NULL, someValueRef) doesn't work.
			
			void	UnprotectValue( JS4D::ValueRef inValue) const;
*/
	
private:
			JS4D::ObjectRef			fObject;
};


//======================================================


class XTOOLBOX_API VJSParms_withArguments : public virtual VJSParms_withException
{
public:
	VJSParms_withArguments( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
		: VJSParms_withException( inContext, inObject, outException)
		, fArgumentCount( inArgumentCount)
		, fArguments( inArguments)
			{
			;
			}

			size_t				CountParams() const					{ return fArgumentCount;}
			
			// cast specified param into some type
			// returns false if index out of bound
			// index starts at 1.
			bool				GetStringParam( size_t inIndex, XBOX::VString& outString) const;
			bool				GetLongParam( size_t inIndex, sLONG *outValue) const;
			bool				GetLong8Param( size_t inIndex, sLONG8 *outValue) const;
			bool				GetULongParam( size_t inIndex, uLONG *outValue) const;
			bool				GetRealParam( size_t inIndex, Real *outValue) const;
			bool				GetBoolParam( size_t inIndex, bool *outValue) const;
			bool				GetBoolParam( size_t inIndex, const XBOX::VString& inNameForTrue, const XBOX::VString& inNameForFalse, size_t* outIndex = NULL) const;

			XBOX::VFile*		RetainFileParam( size_t inIndex, bool allowPath = true) const;
			XBOX::VFolder*		RetainFolderParam( size_t inIndex, bool allowPath = true) const;

			VJSValue			GetParamValue( size_t inIndex) const;
			XBOX::VValueSingle*	CreateParamVValue( size_t inIndex, bool simpleDate = false) const;
			bool				GetParamObject( size_t inIndex, VJSObject& outObject) const;
			bool				GetParamArray( size_t inIndex, VJSArray& outArray) const;
			bool				GetParamFunc( size_t inIndex, VJSObject& outObject, bool allowString = false) const;
			bool				GetParamJSONValue( size_t inIndex, VJSONValue& outValue) const;
			
			// returns true if param doesn't exist or is null (no exception is thrown)
			bool				IsNullParam( size_t inIndex) const;
			
			// tells how is defined the javascript param
			bool				IsNumberParam( size_t inIndex) const;
			bool				IsBooleanParam( size_t inIndex) const;
			bool				IsStringParam( size_t inIndex) const;
			bool				IsObjectParam( size_t inIndex) const;
			bool				IsArrayParam( size_t inIndex) const;
			bool				IsFunctionParam( size_t inIndex) const;

			// returns the private data of specified param if it is a native object of class NATIVE_CLASS, NULL if it is not.
			// exception if index out of bound.
			template<class NATIVE_CLASS>
			typename NATIVE_CLASS::PrivateDataType *GetParamObjectPrivateData( size_t inIndex) const
			{
				VJSValue value( GetParamValue( inIndex));
				return value.GetObjectPrivateData<NATIVE_CLASS>();
			}
			
private:
			size_t				fArgumentCount;
			const JS4D::ValueRef*	fArguments;
};


//======================================================

class XTOOLBOX_API VJSParms_callStaticFunction : public VJSParms_withReturnValue, public VJSParms_withArguments
{
public:
	VJSParms_callStaticFunction( JS4D::ContextRef inContext, JS4D::ObjectRef inFunction, JS4D::ObjectRef inThis, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
		: VJSParms_withArguments( inContext, inFunction, inArgumentCount, inArguments, outException)
		, VJSParms_withReturnValue( inContext, inFunction, outException)
		, VJSParms_withException( inContext, inFunction, outException)
		, fThis( inContext, inThis)
			{
			;
			}

			const VJSObject&	GetThis() const						{ return fThis;}
			void				ReturnThis()						{ fReturnValue = fThis;}

private:
			VJSObject			fThis;
};


//======================================================

class XTOOLBOX_API VJSParms_callAsFunction : public VJSParms_withReturnValue, public VJSParms_withArguments
{
public:
	VJSParms_callAsFunction( JS4D::ContextRef inContext, JS4D::ObjectRef inFunction, JS4D::ObjectRef inThis, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
		: VJSParms_withArguments( inContext, inFunction, inArgumentCount, inArguments, outException)
		, VJSParms_withReturnValue( inContext, inFunction, outException)
		, VJSParms_withException( inContext, inFunction, outException)
		, fFunction (inContext, inFunction)
		, fThis( inContext, inThis)
			{
			;
			}

			// Use the returned (function) object to retrieve private C++ data.

			VJSObject			GetFunction ()						{ return fFunction;	}


			const VJSObject&	GetThis() const						{ return fThis;}
			void				ReturnThis()						{ fReturnValue = fThis;}

private:

			VJSObject			fFunction;		// Object which is called as a function.
			VJSObject			fThis;
};

//======================================================

class XTOOLBOX_API VJSParms_callAsConstructor : public VJSParms_withArguments
{
public:
	VJSParms_callAsConstructor( JS4D::ContextRef inContext, JS4D::ObjectRef inConstructor, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
		: VJSParms_withArguments( inContext, inConstructor, inArgumentCount, inArguments, outException)
		, VJSParms_withException( inContext, inConstructor, outException)
		, fConstructedObject( inContext)
			{
			;
			}
			
			void				ReturnConstructedObject( const VJSObject& inObject)		{ fConstructedObject = inObject;}
			void				ReturnUndefined()										{ fConstructedObject.SetUndefined(); }
			const VJSObject&	GetConstructedObject() const							{ return fConstructedObject;}

private:
			VJSObject			fConstructedObject;
};


//======================================================

class XTOOLBOX_API VJSParms_construct : public VJSParms_withArguments
{
/*
	Used for instantiating an object with new operator VJSGlobalClass::AddConstructorObjectStaticValue has been used.
*/
public:
	typedef void (*fn_construct)( VJSParms_construct&);
	template<fn_construct fn>
	static JS4D::ObjectRef __cdecl Callback( JS4D::ContextRef inContext, JS4D::ObjectRef inConstructor, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
	{
		VJSParms_construct parms( inContext, inConstructor, inArgumentCount, inArguments, outException);
		fn( parms);
		return parms.GetConstructedObject();
	}

	VJSParms_construct( JS4D::ContextRef inContext, JS4D::ObjectRef inConstructor, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
		: VJSParms_withArguments( inContext, inConstructor, inArgumentCount, inArguments, outException)
		, VJSParms_withException( inContext, inConstructor, outException)
		, fConstructedObject( inContext)
			{
			;
			}
			
			void				ReturnConstructedObject( const VJSObject& inObject)		{ fConstructedObject = inObject;}
			void				ReturnUndefined()										{ fConstructedObject.SetUndefined(); }
			const VJSObject&	GetConstructedObject() const							{ return fConstructedObject;}


private:
			VJSObject			fConstructedObject;
};

//======================================================


class XTOOLBOX_API VJSParms_getProperty : public VJSParms_withReturnValue
{
public:
	VJSParms_getProperty( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName, JS4D::ValueRef* outException)
		: VJSParms_withReturnValue( inContext, inObject, outException)
		, VJSParms_withException( inContext, inObject, outException)
		, fPropertyName( inPropertyName)
		{
		#if VERSIONDEBUG
			JS4D::StringRefToUTF8CString( inPropertyName, fDebugPropertyName, sizeof( fDebugPropertyName));
		#endif
		}

			bool				GetPropertyName( XBOX::VString& outPropertyName) const	{ return JS4D::StringToVString( fPropertyName, outPropertyName);}
			bool				GetPropertyNameAsLong( sLONG *outValue) const			{ return JS4D::StringToLong( fPropertyName, outValue);}
			const UniChar*		GetPropertyNamePtr( XBOX::VIndex *outLength) const;

			// Checks static values and so on, instead.

			void				ForwardToParent () 										{ fReturnValue.SetValueRef(NULL);	}
			
			
private:
			JS4D::StringRef		fPropertyName;
			#if VERSIONDEBUG
			char				fDebugPropertyName[256];
			#endif
};


//======================================================


class XTOOLBOX_API VJSParms_setProperty : public VJSParms_withException
{
public:
	VJSParms_setProperty( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName, JS4D::ValueRef inValue, JS4D::ValueRef* outException)
		: VJSParms_withException( inContext, inObject, outException)
		, fPropertyName( inPropertyName)
		, fValue( inContext, inValue)
		{
		#if VERSIONDEBUG
			JS4D::StringRefToUTF8CString( inPropertyName, fDebugPropertyName, sizeof( fDebugPropertyName));
		#endif
		}
	
			bool				GetPropertyName( XBOX::VString& outPropertyName) const	{ return JS4D::StringToVString( fPropertyName, outPropertyName);}
			bool				GetPropertyNameAsLong( sLONG *outValue) const			{ return JS4D::StringToLong( fPropertyName, outValue);}

			const VJSValue&		GetPropertyValue() const								{ return fValue;}
			
			XBOX::VValueSingle*	CreatePropertyVValue(bool simpleDate) const							{ return fValue.CreateVValue( fException, simpleDate);}

			// returns the private data of the property if it is a native object of class NATIVE_CLASS, NULL if it is not.
			template<class NATIVE_CLASS>
			typename NATIVE_CLASS::PrivateDataType *GetPropertyObjectPrivateData( ) const
			{
				if (fValue != NULL)
				{
					JS4D::ClassRef classRef = NATIVE_CLASS::Class();
					return static_cast<typename NATIVE_CLASS::PrivateDataType*>( JS4D::ValueIsObjectOfClass( GetContextRef(), fValue, classRef) ? JS4D::GetObjectPrivate( JS4D::ValueToObject( GetContextRef(), fValue, fException)) : NULL);
				}
				else
					return NULL;
			}

			bool				IsValueNull() const				{ return fValue.IsNull();}
			bool				IsValueNumber() const			{ return fValue.IsNumber();}
			bool				IsValueBoolean() const			{ return fValue.IsBoolean();}
			bool				IsValueString() const			{ return fValue.IsString();}
			bool				IsValueObject() const			{ return fValue.IsObject();}
			
private:
			JS4D::StringRef		fPropertyName;
			VJSValue			fValue;
			#if VERSIONDEBUG
			char				fDebugPropertyName[256];
			#endif
};


//======================================================


class XTOOLBOX_API VJSParms_getPropertyNames : public VJSParms_withContext
{
public:
	VJSParms_getPropertyNames( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::PropertyNameAccumulatorRef inPropertyNames)
		: VJSParms_withContext( inContext, inObject)
		, fPropertyNames( inPropertyNames)
		{
		}
	
			void	ReturnNumericalRange( size_t inBegin, size_t inEnd);
			void	AddPropertyName( const XBOX::VString& inPropertyName);

private:
	JS4D::PropertyNameAccumulatorRef	fPropertyNames;
};

//======================================================

class XTOOLBOX_API VJSParms_hasProperty : public VJSParms_withContext
{
public:
	VJSParms_hasProperty( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName)
		: VJSParms_withContext( inContext, inObject)
		, fPropertyName( inPropertyName)
		, fReturnedBoolean(false)			// Default is to forward to parent.
		{
		#if VERSIONDEBUG
			JS4D::StringRefToUTF8CString( inPropertyName, fDebugPropertyName, sizeof( fDebugPropertyName));
		#endif
		}

			bool				GetPropertyName( XBOX::VString& outPropertyName) const	{ return JS4D::StringToVString( fPropertyName, outPropertyName);}
			bool				GetPropertyNameAsLong( sLONG *outValue) const			{ return JS4D::StringToLong( fPropertyName, outValue);}
			const UniChar*		GetPropertyNamePtr( XBOX::VIndex *outLength) const;

			// Return true if object has the property. Returning false will "forward to parent", it will checks static values and so on.

			void				ReturnBoolean (bool inBoolean)							{ fReturnedBoolean = inBoolean;	}
			void				ReturnTrue ()											{ fReturnedBoolean = true;		}
			void				ReturnFalse ()											{ fReturnedBoolean = false;		}

			void				ForwardToParent ()										{ ReturnFalse();				}

			bool				GetReturnedBoolean ()									{ return fReturnedBoolean;		}
			
private:			
			JS4D::StringRef		fPropertyName;
			#if VERSIONDEBUG
			char				fDebugPropertyName[256];
			#endif
			bool				fReturnedBoolean;
};

//======================================================


class XTOOLBOX_API VJSParms_deleteProperty : public VJSParms_withException
{
public:
	VJSParms_deleteProperty( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName, JS4D::ValueRef* outException)
		: VJSParms_withException( inContext, inObject, outException)
		, fPropertyName( inPropertyName)
		, fReturnedBoolean(true)			// Default is successful deletion.
		{
		#if VERSIONDEBUG
			JS4D::StringRefToUTF8CString( inPropertyName, fDebugPropertyName, sizeof( fDebugPropertyName));
		#endif
		}

			bool				GetPropertyName( XBOX::VString& outPropertyName) const	{ return JS4D::StringToVString( fPropertyName, outPropertyName);}
			bool				GetPropertyNameAsLong( sLONG *outValue) const			{ return JS4D::StringToLong( fPropertyName, outValue);}
			const UniChar*		GetPropertyNamePtr( XBOX::VIndex *outLength) const;

			void				ReturnBoolean (bool inBoolean)							{ fReturnedBoolean = inBoolean;	}
			void				ReturnTrue ()											{ fReturnedBoolean = true;		}
			void				ReturnFalse ()											{ fReturnedBoolean = false;		}

			bool				GetReturnedBoolean ()									{ return fReturnedBoolean;		}
			
private:
			JS4D::StringRef		fPropertyName;
			#if VERSIONDEBUG
			char				fDebugPropertyName[256];
			#endif
			bool				fReturnedBoolean;
};

class XTOOLBOX_API VJSParms_hasInstance : public VJSParms_withException
{
public:

	// Callback for "instanceof".
	//
	//		inObjectToTest instanceof inPossibleConstructor
	//
	// Note that you should have assert(inObjectToTest == GetObject.GetObjectRef()).

	VJSParms_hasInstance(JS4D::ContextRef inContext, JS4D::ObjectRef inPossibleConstructor, JS4D::ValueRef inObjectToTest, JS4D::ValueRef *outException)
	: VJSParms_withException(inContext, (JS4D::ObjectRef) inObjectToTest, outException)
	, fPossibleConstructor((JS4D::ObjectRef) inPossibleConstructor), fObjectToTest((JS4D::ObjectRef) inObjectToTest)
	{
	}

	XBOX::JS4D::ObjectRef	GetPossibleContructor ()	const	{	return fPossibleConstructor;	}	
	XBOX::JS4D::ObjectRef	GetObjectToTest ()			const	{	return fObjectToTest;	}	
			
private:

	XBOX::JS4D::ObjectRef	fPossibleConstructor;
	XBOX::JS4D::ObjectRef	fObjectToTest;
};

//======================================================

// Template for creating JSObjectCallAsFunctionCallback callbacks.

template<class PRIVATE_DATA_TYPE, void (*callback) (VJSParms_callStaticFunction &, PRIVATE_DATA_TYPE *)>
static JS4D::ValueRef __cdecl js_callback (JS4D::ContextRef inContext, JS4D::ObjectRef inFunction, JS4D::ObjectRef inThis, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
{
	XBOX::VJSParms_callStaticFunction	parms(inContext, inFunction, inThis, inArgumentCount, inArguments, outException);

	callback(parms, (PRIVATE_DATA_TYPE *) JS4D::GetObjectPrivate(inThis));

	return (parms.GetReturnValue() != NULL) ? parms.GetReturnValue() : JS4D::MakeNull( inContext);
}

// Template for creating JSObjectCallAsConstructorCallback callbacks.

template<void (*constructor) (VJSParms_callAsConstructor &)>
static JS4D::ObjectRef __cdecl js_constructor (JS4D::ContextRef inContext, JS4D::ObjectRef inConstructor, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
{
	VJSParms_callAsConstructor	parms(inContext, inConstructor, inArgumentCount, inArguments, outException);

	constructor(parms);
	
	return parms.GetConstructedObject().GetObjectRef();	
}

//======================================================

// to force single instantiation of sClass static variable (doesn't work on inline static member function, that's why one need another non-member function)
#if COMPIL_GCC
#define GCC_VISIBILITY_DEFAULT __attribute__((__visibility__("default")))
#else
#define GCC_VISIBILITY_DEFAULT
#endif

template<typename DERIVED> JS4D::ClassRef JS4DGetClassRef() GCC_VISIBILITY_DEFAULT;
template<typename DERIVED> JS4D::ClassRef JS4DGetClassRef()
{
	static JS4D::ClassRef sClass = NULL;
	if (sClass == NULL)
	{
		JS4D::ClassDefinition def = {0};
		DERIVED::GetDefinition( def);
		sClass = JS4D::ClassCreate( &def);
	}
	return sClass;
}


template<class DERIVED, class PRIVATE_DATA_TYPE>
class VJSClass
{
	typedef void (*fn_initialize)( const VJSParms_initialize&, PRIVATE_DATA_TYPE*);
	typedef void (*fn_finalize)( const VJSParms_finalize&, PRIVATE_DATA_TYPE*);
	typedef void (*fn_getProperty)( VJSParms_getProperty&, PRIVATE_DATA_TYPE*);
	typedef bool (*fn_setProperty)( VJSParms_setProperty&, PRIVATE_DATA_TYPE*);
	typedef void (*fn_getPropertyNames)( VJSParms_getPropertyNames&, PRIVATE_DATA_TYPE*);
	typedef void (*fn_hasProperty)( VJSParms_hasProperty&, PRIVATE_DATA_TYPE*);
	typedef void (*fn_deleteProperty)( VJSParms_deleteProperty&, PRIVATE_DATA_TYPE*);
	typedef void (*fn_callStaticFunction)( VJSParms_callStaticFunction&, PRIVATE_DATA_TYPE*);
	typedef void (*fn_callAsFunction)( VJSParms_callAsFunction&);
	typedef void (*fn_callAsConstructor)( VJSParms_callAsConstructor&);
	typedef bool (*fn_hasInstance) (const VJSParms_hasInstance &);
	
public:
	typedef	JS4D::StaticValue			StaticValue;
	typedef	JS4D::StaticFunction		StaticFunction;
	typedef	JS4D::ClassDefinition		ClassDefinition;
	typedef	PRIVATE_DATA_TYPE			PrivateDataType;

	template<fn_initialize fn>
	static void __cdecl js_initialize( JS4D::ContextRef inContext, JS4D::ObjectRef inObject)
	{
		VJSParms_initialize parms( inContext, inObject);
		fn( parms, (PrivateDataType*) JS4D::GetObjectPrivate( inObject)); 
	}

	template<fn_finalize fn>
	static void __cdecl js_finalize( JS4D::ObjectRef inObject)
	{
		VJSParms_finalize parms( inObject);
		fn( parms, (PrivateDataType*) JS4D::GetObjectPrivate( inObject));
	}

	template<fn_getProperty fn>
	static JS4D::ValueRef __cdecl js_getProperty( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName, JS4D::ValueRef* outException)
	{
		VJSParms_getProperty parms( inContext, inObject, inPropertyName, outException);
		fn( parms, (PrivateDataType*) JS4D::GetObjectPrivate( inObject));
		return parms.GetReturnValue();
	}

	template<fn_setProperty fn>
	static bool __cdecl js_setProperty( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName, JS4D::ValueRef inValue, JS4D::ValueRef* outException)
	{
		VJSParms_setProperty parms( inContext, inObject, inPropertyName, inValue, outException);
		return fn( parms, (PrivateDataType*) JS4D::GetObjectPrivate( inObject));
	}

	template<fn_getPropertyNames fn>
	static void __cdecl js_getPropertyNames( JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::PropertyNameAccumulatorRef inPropertyNames)
	{
		VJSParms_getPropertyNames parms( inContext, inObject, inPropertyNames);
		fn( parms, (PrivateDataType*) JS4D::GetObjectPrivate( inObject));
	}
  
	template<fn_hasProperty fn>
	static bool __cdecl js_hasProperty(JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName)
	{
		VJSParms_hasProperty parms(inContext, inObject, inPropertyName);
		fn(parms, (PrivateDataType *) JS4D::GetObjectPrivate(inObject));
		return parms.GetReturnedBoolean();
	}

	template<fn_deleteProperty fn>
	static bool __cdecl js_deleteProperty(JS4D::ContextRef inContext, JS4D::ObjectRef inObject, JS4D::StringRef inPropertyName, JS4D::ValueRef* outException)
	{
		VJSParms_deleteProperty parms(inContext, inObject, inPropertyName, outException);
		fn(parms, (PrivateDataType *) JS4D::GetObjectPrivate(inObject));
		return parms.GetReturnedBoolean();
	}
			
	template<fn_callStaticFunction fn>
	static JS4D::ValueRef __cdecl js_callStaticFunction( JS4D::ContextRef inContext, JS4D::ObjectRef inFunction, JS4D::ObjectRef inThis, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
	{
		VJSParms_callStaticFunction parms( inContext, inFunction, inThis, inArgumentCount, inArguments, outException);
		fn( parms, (PrivateDataType*) JS4D::GetObjectPrivate( inThis));
		return (parms.GetReturnValue() != NULL) ? parms.GetReturnValue() : JS4D::MakeNull( inContext);
	}
	template<fn_callAsFunction fn>
	static JS4D::ValueRef __cdecl js_callAsFunction( JS4D::ContextRef inContext, JS4D::ObjectRef inFunction, JS4D::ObjectRef inThis, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
	{
		VJSParms_callAsFunction parms( inContext, inFunction, inThis, inArgumentCount, inArguments, outException);
		fn( parms);
		return (parms.GetReturnValue() != NULL) ? parms.GetReturnValue() : JS4D::MakeNull( inContext);
	}

	template<fn_callAsConstructor fn>
	static JS4D::ObjectRef __cdecl js_callAsConstructor( JS4D::ContextRef inContext, JS4D::ObjectRef inConstructor, size_t inArgumentCount, const JS4D::ValueRef inArguments[], JS4D::ValueRef* outException)
	{
		VJSParms_callAsConstructor parms( inContext, inConstructor, inArgumentCount, inArguments, outException);
		fn( parms);
		return parms.GetConstructedObject();
	}

	template<fn_hasInstance fn>
	static bool __cdecl js_hasInstance (JS4D::ContextRef inContext, JS4D::ObjectRef inPossibleConstructor, JS4D::ValueRef inObjectToTest, JS4D::ValueRef *outException)
	{
		VJSParms_hasInstance	parms(inContext, inPossibleConstructor, inObjectToTest, outException);
	
		return fn(parms);
	}

	static	JS4D::ClassRef Class()
	{
		return JS4DGetClassRef<DERIVED>();
	}

	static	VJSObject CreateInstance( JS4D::ContextRef inContext, PrivateDataType *inData)
	{
		return VJSObject( inContext, JS4D::MakeObject( inContext, Class(), inData));
	}

};


END_TOOLBOX_NAMESPACE


#endif
