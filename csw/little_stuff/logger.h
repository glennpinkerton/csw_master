/*
 * Header file for Glenn Pinkerton's "homework" assignment.
 * These classes are defined to implement the message logger
 * functionality.
 *
 *  class LoggerGUI - This is an abstract class that defines a single
 *                   pure virtual method for adding a message to a GUI
 *                   text object.
 *
 *  class LogRequest  -  This class encapsulates the various properties
 *                   of the message to be logged, including the type of
 *                   message (info, warning, error), the message text,
 *                   and the logger definition (destination and format
 *                   pair).
 *
 *  class MsgLog  -  This class is instantiated by application code 
 *                   and its public methods can be used to send messages
 *                   for information, warning, and error.
 *
 *  class MsgLogService  -  The actual formatting of the message and 
 *                   output to its destination is done here, based on 
 *                   the parameters in a LogRequest object.
 *
 * The private parts of these three classes are made visible to each
 * other as needed via friend declarations.  The application code using
 * the logging system is limited to the public interface.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <pthread.h>
#include <string>
#include <vector>
#include <map>

#include "tags.h"

#define _DEFAULT_NAME_ "_default_"


namespace logger
{

    enum LoggerMsgType 
    {
        INFO_MSG_TYPE,
        WARNING_MSG_TYPE,
        ERROR_MSG_TYPE
    };

    enum DestinationType
    {
        STD_OUT_DEST,
        FILE_DEST,
        BIT_BUCKET_DEST,
        GUI_DEST
    };


    class MsgLogService;


    class LoggerGUI {
        virtual ~LoggerGUI ();
        virtual void AddLoggerText (const std::string &msg) = 0;
    };

    class LogRequest 
    {
        friend class MsgLogService;
        friend class MsgLog;

      private:

        std::string  message;
        LoggerMsgType  message_type;
        std::string  definition_name;

    };


    class MsgLog
    {
        friend class LogRequest;
        friend class MsgLogService;

      private:

        std::string     defname;
        MsgLogService   *service;

      public:

        MsgLog ();
        MsgLog (std::string definition_name);

        void LogInfoMsg (const std::string &msg);

        void LogWarningMsg (const std::string &msg);

        void LogErrorMsg (const std::string &msg);

      private:

        void RunLoggerThread (const std::string &msg,
                              LoggerMsgType ltype);
        void init ();

    };


    class MsgLogService
    {
        friend class LogRequest;
        friend class MsgLog;

      private:

      //
      // The following inner classes are only used inside the MsgLogService 
      // class.
      //
        class FormatDef 
        {
          public:

            std::string     name;
            std::string     info_fmt_string;
            std::string     warning_fmt_string;
            std::string     error_fmt_string;

            FormatDef () {}
            FormatDef (std::string sname,
                       std::string sinfo_fmt,
                       std::string swarn_fmt,
                       std::string serr_fmt) :
                name (sname),
                info_fmt_string (sinfo_fmt), 
                warning_fmt_string (swarn_fmt), 
                error_fmt_string (serr_fmt) 
            {}
        };

        class DestinationDef
        {
          public:

            std::string     name;
            DestinationType type;
            std::string     dest_string;

            DestinationDef () {}
            DestinationDef (std::string sname, 
                            DestinationType dtype,
                            std::string sdest) :
                name (sname),
                type (dtype),      
                dest_string (sdest)
            {}
        };

        class LoggerPair 
        {
          public:

            FormatDef       *fmt_def;
            DestinationDef  *dest_def;

            LoggerPair (FormatDef *fmt, DestinationDef *dest) :
                fmt_def (fmt),
                dest_def (dest) 
            {}
        };

        class LoggerDef
        {
          public:

            std::string     name;
            LoggerPair      *pair;

            LoggerDef () {}
            LoggerDef (std::string sname, LoggerPair *lpair) :
                name (sname),
                pair (lpair) 
            {}
        };

      private:

        static MsgLogService*  instance;

        MsgLogService ();

        void SetupDefaultDefs ();
        void LoadDefsFromFile ();

        void AddExitThread (pthread_t thread_id);

        void ProcessLogRequest (LogRequest *request);

      public:

        void AddLoggerGUI (std::string name,
                       const LoggerGUI &gui);
        static MsgLogService* GetInstance ();
        static void CleanupThreads (void);

        static void* PrintRequestFunc (void *ptr);
 
      private:

        std::map<std::string, LoggerGUI*> gui_map; 
        std::map<std::string, FormatDef*> format_map;
        std::map<std::string, DestinationDef*> dest_map;
        std::map<std::string, LoggerDef*> logger_map;

        std::vector<pthread_t> exit_thread_list;

        TagReplacer    tag_replacer;

    };

}  // end of namespace logger

#endif
    
