// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libplatform/libplatform.h"
#include "v8.h"
#ifdef _WIN32
//#pragma comment(lib,"./../lib/v8_libbase.dll.lib")
#pragma comment(lib,"./../lib/v8.dll.lib")
#pragma comment(lib,"./../lib/v8_libplatform.dll.lib")
#endif
#include <vector>
using namespace v8;

typedef std::vector<char> CharVector;
//read the context from the filea and store the data in the strContext
bool ReadJavaScriptFile(const char * fileName, CharVector& chVector)
{
	bool bResult = false;
	FILE * fp = fopen(fileName, "r");
	if (NULL != fp)
	{
		char buffer[128];
		int length = 0;
		memset(buffer, sizeof(char), sizeof(buffer));
		while ((length = fread(buffer, sizeof(char), sizeof(buffer), fp)) > 0)
		{
			for (int i = 0; i < length; i++)
			{
				chVector.push_back(buffer[i]);
			}
		}
		bResult = true;
		fclose(fp);
	}
	return bResult;
}

bool RunJavaScriptCode(CharVector& cvContext, v8::Isolate* isolate )
{
	// Create a stack-allocated handle scope. 
	//HandleScope handle_scope;
	// Create a new context. 
	//Handle<Context> context = Context::New();
	// Enter the created context for compiling and 
	// running the hello world script.
	//Context::Scope context_scope(context);
	// Create a string containing the JavaScript source code.
	int length = cvContext.size() + 1;
	char * pChar = new char[length];
	if (NULL != pChar)
	{
		for (int i = 0; i < length - 1; i++)
		{
			pChar[i] = cvContext[i];
		}
		pChar[length - 1] = '\0';
		const char * pcChar = pChar;
		Handle<String> source = String::NewFromUtf8(isolate,pcChar);
		// Compile the source code. 
		Handle<Script> script = Script::Compile(source);
		// Run the script to get the result. 
		Handle<Value> result = script->Run();
		// Convert the result to an ASCII string and print it. 
		String::Utf8Value ascii(result);
		printf("%s \n", *ascii);
	}
	return true;
}


/************************************************************************/
/* javascript call C++ API                                              */
/************************************************************************/
static void GetSum(const FunctionCallbackInfo<Value>& args)
{
	
	if (args.Length() != 0)
	{
		int sum = 0;
		for (int i = 0; i < args.Length(); i++)
		{
			sum += args[i]->Int32Value();
		}
		printf("the sum of arguements is %d\n", sum);
		args.GetReturnValue().Set(v8::Number::New(args.GetIsolate(), sum));
		//return Undefined(args.GetIsolate());
	}
	else
	{
		//return Undefined(args.GetIsolate());
	}
}

///*JavaScript call C++ function*/
//bool JavaScriptCallCppFunction(v8::Isolate* isolate)
//{
//	//创建ScopedHandle对象，该对象销毁后，所有的handle都被销毁了
//	HandleScope handle_scope(isolate);
//	//创建ObjectTemplate对象，用于注册C++全局函数给javascript调用
//	Handle<ObjectTemplate> global_templ = ObjectTemplate::New(isolate);
//	//创建运行时环境,也就是说javascript可以由多个运行时环境
//	Handle<Context> exec_context;
//	//javascript source code
//	Handle<String> js_source = String::NewFromUtf8(isolate,"var first = 2;\
//                                            var second = 3;\
//                                            var third = 4;\
//                                            GetSum(2,3,4);");
//	//存放编译后的脚本对象
//	Handle<Script> js_compiled;
//	//注册C++脚本对象到运行环境
//	global_templ->Set(String::NewFromUtf8(isolate,"GetSum"),
//		FunctionTemplate::New(isolate,GetSum));
//
//	//将刚刚注册的C++函数关联到脚本运行环境中
//	exec_context = Context::New(NULL, global_templ);
//
//	//创建运行环境作用域
//	//Context::Scope context_scope(exec_context);
//	//编译脚本代码
//	js_compiled = Script::Compile(js_source);
//	if (js_compiled.IsEmpty()) {
//		printf("");
//		return true;
//	}
//	//运行脚本代码
//	js_compiled->Run();
//	return true;
//}

///*C++ 调用 javascript function*/
//bool CppCallJavaScriptFunction(v8::Isolate* isolate)
//{
//	//创建ScopedHandle对象，该对象销毁后，所有的handle都被销毁了
//	//HandleScope handle_scope;
//	//创建ObjectTemplate对象，用于注册C++全局函数给javascript调用
//	Handle<ObjectTemplate> global_templ = ObjectTemplate::New(isolate);
//	//创建运行时环境,也就是说javascript可以由多个运行时环境
//	//Handle<Context> exec_context;
//	//javascript source code
//	Handle<String> js_source = String::NewFromUtf8(isolate,"function GetSum(firstNum,secondNum,thirdNum)    \
//                                            {                                               \
//                                                  return firstNum+secondNum+thirdNum;       \
//                                            }");
//	//存放编译后的脚本对象
//	Handle<Script> js_compiled;
//	//注册C++脚本对象到运行环境
//	//将刚刚注册的C++函数关联到脚本运行环境中
//	//exec_context = Context::New(NULL, global_templ);
//
//	//创建运行环境作用域
//	//Context::Scope context_scope(exec_context);
//	//编译脚本代码
//	js_compiled = Script::Compile(js_source);
//	if (js_compiled.IsEmpty()) {
//		printf("");
//		return true;
//	}
//	//运行脚本代码
//	js_compiled->Run();
//
//	//与javascript调用C++ Function 不同的地方
//	int argc = 3;
//	Handle<String> js_func_name;
//	Handle<Value>  js_func_val;
//	Handle<Function> js_func;
//	Handle<Value>  argv[3];
//	Handle<Integer> int_x;
//	Handle<Integer> int_y;
//	Handle<Integer> int_z;
//
//	// 这一句是创建函数名对象
//	js_func_name = String::NewFromUtf8(isolate,"GetSum");
//
//	// 从全局运行环境中进行查找，看看是否存在一个叫做GetSum的函数
//	js_func_val = exec_context->Global()->Get(js_func_name);
//	if (!js_func_val->IsFunction())
//	{
//	}
//	else
//	{
//
//		// 利用handle的强制类型转换，把js_func_val转换成一个函数对象
//		js_func = Handle<Function>::Cast(js_func_val);
//
//		// 初始化参数，所有数据都要定义成javascript可以识别的数据类型，例如Integer对象
//		// javascript中是没有内建数据类型的（int, char, short是c/c++中的用的类型） 
//		int_x = Integer::New(2);
//		int_y = Integer::New(3);
//		int_z = Integer::New(4);
//
//		// 把这些对象放到argv数组中去
//		argv[0] = int_x;
//		argv[1] = int_y;
//		argv[2] = int_z;
//
//		// 利用函数对象去调用该函数，当然需要传入脚本的运行环境，以及参数个数和参数的值。
//		Handle<Value> hResult = js_func->Call(exec_context->Global(), argc, argv);
//		v8::Local<v8::Int32> sum = hResult->ToInt32();
//		int result = sum->Int32Value();
//		printf("\nthe result of GetSum function is %d \n", result);
//	}
//	return true;
//}
//
///*设置javascript的内置对象*/

class InnerObject
{
public:
	Handle<Object> js_obj;
	~InnerObject()
	{

	}
	int x = 1;
	static void PrintObjectName()
	{
		printf("\nThis is inner object\n");
	}
	static void PrintAuthor()
	{
		printf("\nThe author is hlx \n");
	}
	//define function for v8
	static void V8PrintObjectName(const FunctionCallbackInfo<Value>& args)
	{
		PrintObjectName();
	}
	static void V8PrintAuthor(const FunctionCallbackInfo<Value>& args)
	{
		PrintAuthor();
	}
	static void ClearWeakIC(
		const v8::WeakCallbackInfo<InnerObject>& data) {
		printf("clear weak is called\n");
		InnerObject *native = data.GetParameter();
		delete[] native;
		//Local<Object> native_ptr = data.GetParameter()->Get(data.GetIsolate());

		//Local<External> wrap = Local<External>::Cast(native_ptr->GetInternalField(0));
		//InnerObject* native = static_cast<InnerObject*>(native_ptr->Value());
	}
	static void CreateObject(const FunctionCallbackInfo<Value>& args)
	{
		if (!args.IsConstructCall())return;
		InnerObject* obj = new InnerObject();
		obj->x = 4;
		Local<External> native_ptr = External::New(args.GetIsolate(), obj);
		args.This()->SetInternalField(0, native_ptr);
		v8::Persistent<External> garbage(args.GetIsolate(), native_ptr);
		garbage.SetWeak(obj, &ClearWeakIC, v8::WeakCallbackType::kParameter);
		
	}
	
	static void GetF1(Local<String> property,const PropertyCallbackInfo<Value>& info)
	{
		
	}
	static void SetF1(Local<String> property,Local<Value> value,const PropertyCallbackInfo<void>& info)
	{
		v8::String::Utf8Value utf8(info.GetIsolate(), property);
		printf("%s\n", *utf8);
		Local<Object> self = info.This();
		Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
		void* ptr = wrap->Value();
		InnerObject* native_ptr = static_cast<InnerObject*>(ptr);
		int v = value->Int32Value();
		native_ptr->x = v;
		Handle<String> js_func_name = String::NewFromUtf8(info.GetIsolate(),"test");
		Handle<Value>  js_func_ref = self->Get(js_func_name);

		Handle<Function> js_func = Handle<Function>::Cast(js_func_ref);
		Handle<Value> js_data_value = js_func->Call(self, 0, NULL);
		v8::String::Utf8Value utf82(info.GetIsolate(), js_data_value);
		printf("call test %s\n", *utf82);
	}
};
//
//static InnerObject * pInnerObject;
////把C++的对象作为javascript的内置对象
//bool JavaScriptUseCppObjectAsInnerObject()
//{
//	pInnerObject = new InnerObject();
//	HandleScope handle_scope;
//	//全局对象句柄
//	Handle<ObjectTemplate> global_templ = ObjectTemplate::New();
//
//	//运行环境
//	Handle<Context> exec_context = Context::New(NULL, global_templ);
//	//设置运行环境有效范围
//	Context::Scope context_scope(exec_context);
//	//向javascript中设置全局对象
//	Handle<ObjectTemplate> foo_templ;
//	foo_templ = ObjectTemplate::New();
//	//设置该模板创建的对象有一个内部数据区
//	foo_templ->SetInternalFieldCount(1);
//
//	Handle<Object> foo_class_obj;
//	//使用对象模板创建出一个对象
//	foo_class_obj = foo_templ->NewInstance();
//	//创建一个外部对象
//	Handle<External> foo_class_ptr = External::New(static_cast<InnerObject*>(pInnerObject));
//	//将foo_class_ptr设置到foo_class_obj
//	foo_class_obj->SetInternalField(0, foo_class_ptr);
//	//为当前对象设置其外部函数接口
//	foo_class_obj->Set(v8::String::New("V8PrintObjectName"), FunctionTemplate::New(InnerObject::V8PrintObjectName)->GetFunction());
//	foo_class_obj->Set(v8::String::New("V8PrintAuthor"), FunctionTemplate::New(InnerObject::V8PrintAuthor)->GetFunction());
//	// 这一步是最关键的，从当前的javascript执行环境中获取Global()全局环境对象，然后使用Set函数
//	// 把刚刚初始化完毕的foo_class_obj对象放到javascript的全局执行环境中去
//	exec_context->Global()->Set(String::New("foo"),
//		foo_class_obj,
//		(PropertyAttribute)(v8::ReadOnly));
//	Handle<String> js_source = String::New("foo.V8PrintObjectName();");
//	//编译javascript脚本
//	Handle<Script> js_result = Script::Compile(js_source);
//	js_result->Run();
//	return true;
//}
//int main()
//{
//	int x = 1;
//	JavaScriptCallCppFunction();
//	CppCallJavaScriptFunction();
//	JavaScriptUseCppObjectAsInnerObject();
//	do
//	{
//		printf("Please input the javascript file Name\t");
//		char fileName[32];
//		scanf("%s", fileName);
//		CharVector cvJavaScript;
//		if (ReadJavaScriptFile(fileName, cvJavaScript))
//		{
//			RunJavaScriptCode(cvJavaScript);
//		}
//		cvJavaScript.clear();
//		printf("would you want to Continue? yes enter 1,no enter 0\n");
//		scanf("%d", &x);
//	} while (1 == x);
//	return 0;
//}

#define V8_STRING(arg1,arg2) String::NewFromUtf8(arg1, arg2)



int main(int argc, char* argv[]) {
  // Initialize V8.
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();

  // Create a new Isolate and make it the current one.
  v8::Isolate::CreateParams create_params;
  create_params.array_buffer_allocator =
      v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  v8::Isolate* isolate = v8::Isolate::New(create_params);
  {
    v8::Isolate::Scope isolate_scope(isolate);

    // Create a stack-allocated handle scope.
   
	Handle<ObjectTemplate> global_templ = ObjectTemplate::New(isolate);
	global_templ->Set(String::NewFromUtf8(isolate, "GetSum"),
		FunctionTemplate::New(isolate, GetSum));

	//InnerObject *pInnerObject = new InnerObject();
	//pInnerObject->x = 3;
	//Local<External> native_ptr = External::New(isolate, pInnerObject);
	
	
	Local<FunctionTemplate> class_template = FunctionTemplate::New(isolate, InnerObject::CreateObject);
	class_template->SetClassName(String::NewFromUtf8(isolate, "Foo"));

	//static
	class_template->Set(v8::String::NewFromUtf8(isolate, "V8PrintObjectName"), FunctionTemplate::New(isolate, InnerObject::V8PrintObjectName));
	class_template->Set(v8::String::NewFromUtf8(isolate, "V8PrintAuthor"), FunctionTemplate::New(isolate, InnerObject::V8PrintAuthor));
	//method
	Handle<ObjectTemplate> class_proto = class_template->PrototypeTemplate();
	class_proto->SetAccessor(String::NewFromUtf8(isolate, "f1"), InnerObject::GetF1, InnerObject::SetF1);
	class_template->InstanceTemplate()->SetInternalFieldCount(1);
	
	global_templ->Set(String::NewFromUtf8(isolate, "Foo"), class_template);
    // Create a new context.
    v8::Local<v8::Context> context = v8::Context::New(isolate,0, global_templ);

    // Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);

    // Create a string containing the JavaScript source code.
    v8::Local<v8::String> source =
        v8::String::NewFromUtf8(isolate, "'Hello' + ', World!'",
                                v8::NewStringType::kNormal)
            .ToLocalChecked();
	
	Local<String> get_sum = String::NewFromUtf8(isolate, "var first = 2;\
	                                        var second = 3;\
                                        var third = 4;\
                                        GetSum(2,3,4);function GetJSString(str){return 'i am from js->' + str}\
										Foo.prototype.test=function(){Foo.V8PrintAuthor();};\
										Foo.V8PrintObjectName();\
										 var foo = new Foo(); foo.f1=123;\
										var foo2= new Foo();foo2.f1=222;foo2=null", NewStringType::kNormal).ToLocalChecked();
	
	Local<String> js_func_name = String::NewFromUtf8(isolate, "GetJSString");
	
	Local<String> source2 = String::NewFromUtf8(isolate, "function Point(x,y){this.x=x; this.y=y;} Point.prototype.show=function(){return '(x,y) = '+this.x+','+this.y;}");
	v8::Local<v8::Script> script2 =
		v8::Script::Compile(context, source2).ToLocalChecked();
	script2->Run();

    // Compile the source code.
    v8::Local<v8::Script> script =
        v8::Script::Compile(context, get_sum).ToLocalChecked();

    // Run the script to get the result.
    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
	Local<Value> js_func_val = context->Global()->Get(js_func_name);
	//if(js_func_val->IsFunction())
	Local<Function> js_func= Local<Function>::Cast(js_func_val);
	Local<String> cpp_str = String::NewFromUtf8(isolate,"i am from cpp");
	Handle<Value>  argv[1] = { cpp_str };

	Local<Value> hResult = js_func->Call(context->Global(), 1, argv);
	v8::Local<v8::String> ret_str = hResult->ToString();
	String::Utf8Value ret_str_utf8(isolate, ret_str);
	printf("the result of GetJSString function is %s \n", *ret_str_utf8);
	isolate->LowMemoryNotification();
    // Convert the result to an UTF8 string and print it.
    v8::String::Utf8Value utf8(isolate, result);
    printf("%s\n", *utf8);
  }
  // Dispose the isolate and tear down V8.
  isolate->Dispose();
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  delete create_params.array_buffer_allocator;
  return 0;
}
