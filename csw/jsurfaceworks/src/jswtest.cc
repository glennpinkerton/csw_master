
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/* ANSI C includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* JNI includes */
#include <jni.h>

/* pralibs includes */
#include <pralibs/jni/jni_main.h>
#include <pralibs/jvm/jvm_create.h>

/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
  int arg;
  JAR_FILE_LIST jar_file_list;
  JVM_OPTION_LIST jvm_option_list;
  char bin_dir[256];
  JNIEnv *env;
  JavaVM *jvm;

  for (arg = 1; arg < argc; arg++) {
    if (strcmp(argv[arg], "-?") == 0) {
      printf(
        "USAGE: %s\n",
        argv[0]
      );
      return(EXIT_SUCCESS);
    }
  }

  /* Create the Java Virtual Machine (VM) */
  jar_file_list = new_jar_file_list();
  jar_file_list_append_src_tree_for_testme(jar_file_list);
  jvm_option_list = new_jvm_option_list(2);
  jvm_option_list_append_check_jni(jvm_option_list);
#if 0
  jvm_option_list_append_log_gc(jvm_option_list, "loggc.txt");
#endif
#if 0
  jvm_option_list_append_verbose_jni(jvm_option_list);
#endif
  strcpy(bin_dir, _SRC_TREE_);
  strcat(bin_dir, "/csw/jsurfaceworks/src");
  jvm_create(
    "." /* root_dir */,
    bin_dir,
    "." /* home_dir */,
    jar_file_list,
    JRE_TYPE_SUN_1_4_1,
    jvm_option_list,
    &jvm,
    &env
  );
  delete_jar_file_list(jar_file_list);
  delete_jvm_option_list(jvm_option_list);
  if (jvm == NULL) {
    return(EXIT_FAILURE);
  }

  java_call_main_method(
    env,
    "csw/jsurfaceworks/src/UnitTest" /* class_name */,
    "main" /* method_name */,
    argc,
    (char const **) argv
  );

  jvm_destroy(jvm, env);

  return(EXIT_SUCCESS);
}
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
