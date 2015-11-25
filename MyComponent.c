/*	File:		MyComponent.c	Contains:	simple component sample.	Written by: John Wang		Copyright:	Copyright � 1994-1999 by Apple Computer, Inc., All Rights Reserved.				You may incorporate this Apple sample source code into your program(s) without				restriction. This Apple sample source code has been provided "AS IS" and the				responsibility for its operation is yours. You are not permitted to redistribute				this Apple sample source code as "Apple sample source code" after having made				changes. If you're going to re-distribute the source, we require that you make				it clear in the source that the code was descended from Apple sample source				code, but that you've made changes.	Change History (most recent first):				7/28/1999	Karl Groethe	Updated for Metrowerks Codewarror Pro 2.1				*/#ifdef THINK_C#define		applec#endif#include	<Memory.h>#include	<Errors.h>#include	<Components.h>#include	<Movies.h>#include	<QuickTimeComponents.h>#include	<Components.h>#include	<LowMem.h>#include	"MyComponent.h"/* ------------------------------------------------------------------------- *///	Component entry point.pascal ComponentResult main(ComponentParameters *params, char **storage){	PrivateGlobals	 	**myPrivateGlobals = (PrivateGlobals **) storage;	long				ret;		if ( kDEBUGME )		DebugStr("\pIn main()");		if ( params->what < 0 ) { 		switch ( params->what ) {			case kComponentOpenSelect:				return ( CallComponentFunction(params, (ComponentFunctionUPP) MyOpen) );				case kComponentCloseSelect:				return ( CallComponentFunctionWithStorage(storage, params,						(ComponentFunctionUPP) MyClose) );							case kComponentCanDoSelect:				ret = CallComponentFunction(params, (ComponentFunctionUPP) MyCanDo);				if ( ret == false ) {					DebugStr("\pIn kComponentCanDoSelect");					if ( (**myPrivateGlobals).delegate ) {						ret = DelegateComponentCall(params, (**myPrivateGlobals).delegate);					}				}				return ( ret );				case kComponentVersionSelect: 				return ( CallComponentFunction(params, (ComponentFunctionUPP) MyVersion) );				case kComponentRegisterSelect: 				return ( CallComponentFunctionWithStorage(storage, params,						(ComponentFunctionUPP) MyRegister) );			case kComponentTargetSelect: 				return ( CallComponentFunctionWithStorage(storage, params,						(ComponentFunctionUPP) MyTarget) );			default:				return ( paramErr );		}	} else {		switch ( params->what ) {			default:				if ( (**myPrivateGlobals).delegate ) {					//	If base media handler is targeted, then delegate all unimplemented					//	calls to the base media handler.					long	ret;										ret = DelegateComponentCall(params, (**myPrivateGlobals).delegate);					return ( ret );				} else {					//	If base media handler has not been targeted, then return paramErr.					return ( paramErr );				}		}	}}/* ------------------------------------------------------------------------- *///	Required component calls.pascal ComponentResult MyOpen(ComponentInstance self){	PrivateGlobals	 		**myPrivateGlobals;	ComponentInstance		myComp;	ComponentDescription	searchComp;	Component				dataHandler;		if ( kDEBUGME )		DebugStr("\pIn MyOpen()");			myPrivateGlobals = nil;	searchComp.componentType = 'dhlr';	searchComp.componentSubType = 'alis';	searchComp.componentManufacturer = 'appl';	searchComp.componentFlags = 0x40000000;	searchComp.componentFlagsMask = 0x40000000;	//	Open base media handler component and target it.	dataHandler = FindNextComponent(nil, &searchComp);	if (dataHandler == nil) {		DebugStr("\pCould not find data handler.");	}	if ((myComp = OpenComponent(dataHandler)) == nil) {		return(componentNotCaptured);	}	ComponentSetTarget(myComp, self);		//	Create private variables.	myPrivateGlobals = (PrivateGlobals **) NewHandleClear(sizeof(PrivateGlobals));	if ( myPrivateGlobals == nil )		goto bail;		//	Initialize private variables.	(**myPrivateGlobals).delegate = myComp;	//	Since we've gotten here, everyt hings ok and we can set up the connection.	SetComponentInstanceStorage(self, (Handle) myPrivateGlobals);	return ( noErr );bail:	if ( myPrivateGlobals )		DisposeHandle((Handle) myPrivateGlobals);	return ( memFullErr );}pascal ComponentResult MyClose(Handle storage, ComponentInstance self){	PrivateGlobals	 	**myPrivateGlobals = (PrivateGlobals **) storage;	if ( kDEBUGME )		DebugStr("\pIn MyClose()");			//	Dispose of private variables.	if ( myPrivateGlobals ) {		if ((**myPrivateGlobals).delegate) {			CloseComponent((**myPrivateGlobals).delegate);			(**myPrivateGlobals).delegate = nil;		}		DisposeHandle((Handle) myPrivateGlobals);	}		return ( noErr );}pascal ComponentResult MyCanDo(short selector){		if ( kDEBUGME )		DebugStr("\pIn MyCanDo()");		switch ( selector ) {			//	Required component calls.		case kComponentOpenSelect:		case kComponentCloseSelect:		case kComponentCanDoSelect:		case kComponentVersionSelect: 		case kComponentRegisterSelect: 		case kComponentTargetSelect: 		//	MyComponent specific calls.		//	Not handled.		default:			return ( false );	}}pascal ComponentResult MyVersion(){	if ( kDEBUGME )		DebugStr("\pIn MyVersion()");	return ( (kMyComponentSpec<<16) | (kMyComponentVersion) );}pascal ComponentResult MyRegister(){	return ( false );}pascal ComponentResult MyTarget(Handle storage, ComponentInstance self){	PrivateGlobals	 	**myPrivateGlobals = (PrivateGlobals **) storage;	if ( kDEBUGME )		DebugStr("\pIn MyTarget()");	//	From now on, self will be the component instance that targeted us.	(**myPrivateGlobals).self = self;		return ( noErr );}