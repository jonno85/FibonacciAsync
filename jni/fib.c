#include "com_fib_FibActivity.h"
#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#include <android/log.h>

#define LOG_TAG "FIB"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

JavaVM * g_vm;
jobject g_obj;
jmethodID g_mid;
jclass g_clazz;

long number;
pthread_t th;

typedef enum { false, true } bool;

jlong fib(jlong n) {
	if(n==0) return 0;
	if(n==1) return 1;
	return fib(n-1)+fib(n-2);
}

void CHECK(const char* msg, jclass toCheck) {
	if (toCheck == NULL) {
		LOGE("%s: Error retrieving jclass", msg);
	}
}

void callback(int val) {
	JNIEnv * env;
	// double check it's all ok
	int getEnvStat = (*g_vm)->GetEnv((JavaVM*)g_vm, (void *)&env, JNI_VERSION_1_6);
	if (getEnvStat == JNI_EDETACHED) {
		LOGW("GetEnv: not attached\n");
		if ((*g_vm)->AttachCurrentThread((JavaVM*)g_vm, (JNIEnv **) &env, NULL) != 0) {
			LOGW("Failed to attach\n");
		}
	} else if (getEnvStat == JNI_OK) {
		LOGW("OK");
	} else if (getEnvStat == JNI_EVERSION) {
		LOGW("GetEnv: version not supported\n");
	}
	LOGW("Call Void Method ");

	(*env)->CallVoidMethod(env, g_obj, g_mid, val);
	//(*g_env)->CallStaticVoidMethod(g_env, g_clazz, g_mid, val);
	LOGW("Call ExceptionCheck Method");
	if ((*env)->ExceptionCheck(env)) {
		(*env)->ExceptionDescribe(env);
	}

	(*g_vm)->DetachCurrentThread(g_vm);
}

void* calculate(void *ptr){
	long v = fib(number);
	LOGW("value calculate %ld", v);
	callback(v);
	LOGW("called callback");
	return NULL;
}

JNIEXPORT jlong JNICALL Java_com_fib_FibActivity_fibN
  (JNIEnv *env, jclass clazz, jlong n) {
	return fib(n);
}

JNIEXPORT jboolean JNICALL Java_com_fib_FibActivity_asyncFibN
  (JNIEnv *env, jclass clazz, jlong n)
{
	bool returnValue = true;

	number = n;
	LOGW("number %ld", number);
	pthread_create(&th, NULL, calculate, NULL);

	return (jboolean)returnValue;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	g_vm = vm;
	JNIEnv* env;
	(*vm)->GetEnv((JavaVM*)g_vm, (void *)&env, JNI_VERSION_1_6);
	if (env == NULL) {
		LOGD("Failed to get the environment using GetEnv()");
		return -1;
	}

	jclass localClass = (*env)->FindClass(env, "com/fib/FibActivity");
	if (localClass == NULL) {
		LOGW("Failed to find class\n");
	}

	g_clazz = (jclass)((*env)->NewGlobalRef(env, localClass));
	CHECK("FibActivity", g_clazz);
	(*env)->DeleteLocalRef(env, localClass);

	//init class object
	const jmethodID init_method = (*env)->GetMethodID(env, g_clazz, "<init>", "()V");

	// create reference to the object
	const jobject obj = (*env)->NewObject(env, g_clazz, init_method);
	g_obj = (jobject)(*env)->NewGlobalRef(env, obj);

	(*env)->DeleteLocalRef(env, obj);

	g_mid = (*env)->GetMethodID(env, g_clazz, "callback", "(I)V");
	if (g_mid == NULL) {
		LOGW("Unable to get method ref\n");
	}

	return JNI_VERSION_1_6;
}

void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved){
	if(g_vm != NULL){
		JNIEnv* env;
		(*g_vm)->GetEnv((JavaVM*)g_vm, (void *)&env, JNI_VERSION_1_6);
		if (env != NULL) {
			(*env)->DeleteGlobalRef(env, g_clazz);
			(*env)->DeleteGlobalRef(env, g_obj);
			(*env)->DeleteGlobalRef(env, g_mid);
			g_mid = 0;
		}
	}
}
