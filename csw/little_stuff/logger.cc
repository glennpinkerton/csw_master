
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "logger.h"

using namespace logger;

MsgLogService *MsgLogService::instance = NULL;

pthread_mutex_t logger_mutex = PTHREAD_MUTEX_INITIALIZER;

/******************************************************************************/

/*
 * Methods for the MsgLog class.
 */

/*
 * Empty constructor that uses the _DEFAULT_NAME_ logging definition.
 */
MsgLog::MsgLog ()
{
    defname = _DEFAULT_NAME_;
    init ();
}


/*
 * Constructor that specifies the logging definition name as a
 * std::string object.  If the string object is 
 * empty, use the _DEFAULT_NAME_ logger definition.
 */
MsgLog::MsgLog (std::string str)
{
    if (str.size() == 0) {
        str = _DEFAULT_NAME_;
    }

    defname = str;
    init ();
}


void MsgLog::init ()
{
    service = MsgLogService::GetInstance ();
}
        

void MsgLog::LogInfoMsg (const std::string &msg)
{
    RunLoggerThread (msg, INFO_MSG_TYPE);
}

void MsgLog::LogWarningMsg (const std::string &msg)
{
    RunLoggerThread (msg, WARNING_MSG_TYPE);
}

void MsgLog::LogErrorMsg (const std::string &msg)
{
    RunLoggerThread (msg, ERROR_MSG_TYPE);
}


/*
 * Create a LogRequest object on the heap.  Populate it with the message,
 * message type, and definition name.  Then, start a posix thread, passing
 * the pointer to the LogRequest object to the static PrintRequestFunc
 * method of the MessageLogService class.
 */
void MsgLog::RunLoggerThread (const std::string &msg,
                              LoggerMsgType ltype)
{
    LogRequest *request;

    try {
        request = new LogRequest ();
    }
    catch (...) {
        throw 
          ("Exception createing a new LogRequest object "
           "in the MsgLog::RunLoggerThread method");
    }

    request->message = msg;
    request->message_type = ltype;
    request->definition_name = defname;

    pthread_t logthread;

    pthread_create (&logthread, NULL, 
                    MsgLogService::PrintRequestFunc,
                    (void *)request);

    service->AddExitThread (logthread);
}


/*****************************************************************************/

/*   
 * Methods for the MsgLogService class.
 */

/*
 * A single instance of the MsgLogService is allowed per application.
 * Create this instance the first time this is called.  Return the
 * single instance each time this method is called.
 */
MsgLogService* MsgLogService::GetInstance ()
{
    if (instance == NULL) {
        instance = new MsgLogService ();
    }
    return instance;
}


/*
 * Add the specified thread id to a vector which will
 * be scanned at exit time to make sure all threads have
 * completed prior to exiting the program.
 *
 * In the real world, I would make this more sophisticated.
 * For the homework assignment, I leave it simple like this.
 */
void MsgLogService::AddExitThread (pthread_t thread)
{
    if (instance == NULL) return;

    instance->exit_thread_list.push_back(thread);

}


/*
 * This static method is called at exit to make sure all logger threads
 * have finished before exiting the main program.
 */
void MsgLogService::CleanupThreads (void)
{
    if (instance == NULL) return;

    int    i, size;

    size = instance->exit_thread_list.size ();
    for (i=0; i<size; i++) {
        pthread_t  pt = instance->exit_thread_list[i];
        pthread_join (pt, NULL);
    }

    instance->exit_thread_list.clear ();

}


/*
 * Private constructor, only called from the GetInstance static method.
 */
MsgLogService::MsgLogService ()
{
    SetupDefaultDefs ();
    LoadDefsFromFile ();
    atexit (MsgLogService::CleanupThreads);
}

/*
 * This method sets up a single logger format and a single logger destination
 * It defines a single logger pair using these and makes a "default_logger"
 * definition using the pair.  In the real world, if no config file is found,
 * these defaults are the only available logger format/destination.
 */
void MsgLogService::SetupDefaultDefs ()
{

    try {

        FormatDef *fdef = new FormatDef 
                       ("default_format",
                        "Information: %s\n",
                        "Warning: %s\n",
                        "Error: %s\n");
        format_map["default_format"] = fdef;

        DestinationDef *ddef = new DestinationDef
                            ("default_dest",
                             FILE_DEST,
                             "logger.out");
        dest_map["default_dest"] = ddef;

        LoggerPair *lpair = new LoggerPair (fdef, ddef);
        
        LoggerDef *ldef = new LoggerDef ("default_logger", lpair);
        logger_map["default_logger"] = ldef;

    }

    catch (...) {
        throw "Exception setting up MsgLogService defaults.";
    }

}


/*
 * In the real world, this method would find a config file or set of config
 * files to setup the logger formats, destinations, pairs and definitions.
 * For the purpose of this assignment, I choose not to write the config read,
 * parse, etc.  I just hard code a few formats, destinations, pairs and
 * definitions.
 */
void MsgLogService::LoadDefsFromFile ()
{

    try {

        FormatDef *fdef = new FormatDef 
                       ("time_format_1",
                        "[%time%] Information: %s\n",
                        "[%time%] Warning: %s\n",
                        "[%time%] Error: %s\n");
        format_map["time_format_1"] = fdef;

        fdef = new FormatDef 
                       ("xml_format_1",

                        "<logEntry>\n"
                        "  <time>%time%</time>\n"
                        "  <type>INFORMATION</type>\n"
                        "  <message>%s</message>\n"
                        "</logEntry>\n",

                        "<logEntry>\n"
                        "  <time>%time%</time>\n"
                        "  <type>WARNING</type>\n"
                        "  <message>%s</message>\n"
                        "</logEntry>\n",

                        "<logEntry>\n"
                        "  <time>%time%</time>\n"
                        "  <type>ERROR</type>\n"
                        "  <message>%s</message>\n"
                        "</logEntry>\n"
                       );

        format_map["xml_format_1"] = fdef;

        DestinationDef *ddef = new DestinationDef
                            ("home_dest",
                             FILE_DEST,
                             "%home_dir%/logger.out");
        dest_map["home_dest"] = ddef;

        ddef = new DestinationDef
                            ("envar_dest",
                             FILE_DEST,
                             "%logger_envar_file%");
        dest_map["envar_dest"] = ddef;

        LoggerPair *lpair = new LoggerPair (format_map["time_format_1"],
                                            dest_map["home_dest"]);
        LoggerDef *ldef = new LoggerDef ("thread1_logger", lpair);
        logger_map["thread1_logger"] = ldef;

        lpair = new LoggerPair (format_map["xml_format_1"],
                                dest_map["envar_dest"]);
        ldef = new LoggerDef ("thread2_logger", lpair);
        logger_map["thread2_logger"] = ldef;

    }

    catch (...) {
        throw "Exception loading logger definitions from a file.";
    }

}


/*
 * This function is the entry point for the thread that executes
 * a single logger request.
 */
void *MsgLogService::PrintRequestFunc (void *ptr)
{
    if (instance == NULL) return NULL;

    LogRequest *request = (LogRequest *)ptr;
    instance->ProcessLogRequest (request);

    delete request;

    return NULL;
}


/*
 * Process the specified message logger request.  This is called from
 * multiple threads, so a mutex is used for synchronization.
 */
void MsgLogService::ProcessLogRequest (LogRequest *request)
{

  //
  // If the logger definition doesn't exist, use the default definition.
  //
    if (logger_map.find (request->definition_name) == logger_map.end ()) {
        request->definition_name = "default_logger";
    }
    
  //
  // Get the format and destination for this logger definition.
  //
    LoggerDef *ldef = logger_map[request->definition_name];

    LoggerPair *lpair = ldef->pair;
    FormatDef *fmt = lpair->fmt_def;
    DestinationDef *dest = lpair->dest_def;

  //
  // Do nothing if the destination is the bit bucket.
  //
    if (dest->type == BIT_BUCKET_DEST) {
        return;
    }

  //
  // For the homework assignment, also disregard the GUI destination.
  //
    if (dest->type == GUI_DEST) {
        return;
    }

  //
  // Get the appropriate format string for the message type.
  //
    std::string raw_fmt_str;

    switch (request->message_type)
    {

        case INFO_MSG_TYPE:
            raw_fmt_str = fmt->info_fmt_string;
            break;            

        case WARNING_MSG_TYPE:
            raw_fmt_str = fmt->warning_fmt_string;
            break;            

        case ERROR_MSG_TYPE:
            raw_fmt_str = fmt->error_fmt_string;
            break;            

        default:
            raw_fmt_str = "Undefined Format String: %s\n";
            break;            
    }

  //
  // Replace tags in the format string.  For the purposes of this homework,
  // I am assuming that the fmt_str will always have a %s descriptor which
  // will get the actual text of the message.
  //
    std::string fmt_str = tag_replacer.ReplaceTags (raw_fmt_str);

  //
  // The file operations need to be synchronized using the logger_mutex.
  //
    pthread_mutex_lock (&logger_mutex);

  //
  // If the destination is stdout, just use printf to send the message
  // using the format.
  //
    if (dest->type == STD_OUT_DEST) {
        printf (fmt_str.c_str(), request->message.c_str());
        pthread_mutex_unlock (&logger_mutex);
        return;
    }    

  //
  // Get a file name for the destination definition.  Open the file
  // for append access, write the message and close the file.  The
  // repeated open and close for each message is obviously inefficient,
  // but it gives the messages a better chance to actually get to the
  // log file if the program crashes.
  //
    FILE    *fptr;

    std::string path = tag_replacer.ReplaceTags (dest->dest_string);

    fptr = fopen (path.c_str(), "a");
    if (fptr == NULL) {
        printf ("Cannot open log file: %s\n", path.c_str());
        pthread_mutex_unlock (&logger_mutex);
        return;
    }

    fprintf (fptr, fmt_str.c_str(), request->message.c_str());

    fclose (fptr);

    pthread_mutex_unlock (&logger_mutex);

    return;
}
