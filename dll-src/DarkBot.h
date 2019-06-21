#pragma once

/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_github_manolo8_darkbot_core_DarkBotAPI */

#ifndef _Included_com_github_manolo8_darkbot_core_DarkBotAPI
#define _Included_com_github_manolo8_darkbot_core_DarkBotAPI
#ifdef __cplusplus
extern "C" {
#endif
#undef com_github_manolo8_darkbot_core_DarkBotAPI_NO_MOVE
#define com_github_manolo8_darkbot_core_DarkBotAPI_NO_MOVE 2L
#undef com_github_manolo8_darkbot_core_DarkBotAPI_NO_RESIZE
#define com_github_manolo8_darkbot_core_DarkBotAPI_NO_RESIZE 1L
#undef com_github_manolo8_darkbot_core_DarkBotAPI_SHOWN
#define com_github_manolo8_darkbot_core_DarkBotAPI_SHOWN 64L
#undef com_github_manolo8_darkbot_core_DarkBotAPI_HIDDEN
#define com_github_manolo8_darkbot_core_DarkBotAPI_HIDDEN 128L
	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    createWindow0
	* Signature: ()V
	*/
	JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_createWindow0
	(JNIEnv *, jobject, jstring);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    isValid
	* Signature: ()Z
	*/
	JNIEXPORT jboolean JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_isValid
	(JNIEnv *, jobject);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    mousePress
	* Signature: (II)V
	*/
	JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_mousePress
	(JNIEnv *, jobject, jint, jint);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    mouseMove
	* Signature: (II)V
	*/
	JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_mouseMove
	(JNIEnv *, jobject, jint, jint);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    mouseRelease
	* Signature: (II)V
	*/
	JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_mouseRelease
	(JNIEnv *, jobject, jint, jint);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    mouseClick
	* Signature: (II)V
	*/
	JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_mouseClick
	(JNIEnv *, jobject, jint, jint);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    keyboardClick
	* Signature: (C)V
	*/
	JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_keyboardClick
	(JNIEnv *, jobject, jchar);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    readMemoryDouble
	* Signature: (J)D
	*/
	JNIEXPORT jdouble JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_readMemoryDouble
	(JNIEnv *, jobject, jlong);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    readMemoryLong
	* Signature: (J)J
	*/
	JNIEXPORT jlong JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_readMemoryLong
	(JNIEnv *, jobject, jlong);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    readMemoryInt
	* Signature: (J)I
	*/
	JNIEXPORT jint JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_readMemoryInt
	(JNIEnv *, jobject, jlong);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    readMemoryBoolean
	* Signature: (J)Z
	*/
	JNIEXPORT jboolean JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_readMemoryBoolean
	(JNIEnv *, jobject, jlong);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    readMemory
	* Signature: (JI)[B
	*/
	JNIEXPORT jbyteArray JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_readMemory
	(JNIEnv *, jobject, jlong, jint);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    writeMemoryDouble
	* Signature: (JD)V
	*/
	JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_writeMemoryDouble
	(JNIEnv *, jobject, jlong, jdouble);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    writeMemoryLong
	* Signature: (JJ)V
	*/
	JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_writeMemoryLong
	(JNIEnv *, jobject, jlong, jlong);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    writeMemoryInt
	* Signature: (JI)V
	*/
	JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_writeMemoryInt
	(JNIEnv *, jobject, jlong, jint);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    queryMemoryInt
	* Signature: (II)[J
	*/
	JNIEXPORT jlongArray JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_queryMemoryInt
	(JNIEnv *, jobject, jint, jint);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    queryMemoryLong
	* Signature: (JI)[J
	*/
	JNIEXPORT jlongArray JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_queryMemoryLong
	(JNIEnv *, jobject, jlong, jint);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    queryMemory
	* Signature: ([BI)[J
	*/
	JNIEXPORT jlongArray JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_queryMemory
	(JNIEnv *, jobject, jbyteArray, jint);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    pixels
	* Signature: (IIII)[I
	*/
	JNIEXPORT jintArray JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_pixels
	(JNIEnv *, jobject, jint, jint, jint, jint);

	/*
	* Class:     com_github_manolo8_darkbot_core_DarkBotAPI
	* Method:    refresh
	* Signature: ()V
	*/
	JNIEXPORT void JNICALL Java_com_github_manolo8_darkbot_core_DarkBotAPI_refresh
	(JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif