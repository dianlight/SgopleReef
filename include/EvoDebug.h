#pragma once
#include <Arduino.h>
#include <stdarg.h>
#include <exception>
#include <list>
/**
 * @file EvoDebug.h (HeaderOnly Function)
 * @author Lucio Tarantino
 * @brief Singleton Class for debugging
 * @version 0.1
 * @date 2020-02-27
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifdef EVODEBUG
    #define EVOD_CHUNCK_LEN 256
    class EvoAppender: public Print {
        public:
            enum __attribute__((__packed__)) LEVEL {
                DEBUG,
                INFO,
                WARNING,
                ERROR
            };

            virtual void begin() = 0;
            virtual void end() = 0;
            virtual size_t write(uint8_t) = 0;
            virtual size_t write(const uint8_t *buffer, size_t size) = 0;

            size_t vprintf(const char *format, va_list arg) {
                    char temp[64];
                    char* buffer = temp;
                    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
                    if (len > sizeof(temp) - 1) {
                        buffer = new char[len + 1];
                        if (!buffer) {
                            return 0;
                        }
                        vsnprintf(buffer, len + 1, format, arg);
                    }
                    len = write((const uint8_t*) buffer, len);
                    if (buffer != temp) {
                        delete[] buffer;
                    }
                    return len;
            }
    };

    #ifdef EVODEBUG_SERIAL 
        #ifndef log_wn
        class SerialEvoAppender: public EvoAppender {
            public:
                void begin(){
                    Serial.begin(115200);
                    Serial.println("SerialEvoAppender start");
                }
                void end(){
                    Serial.println("SerialEvoAppender stop");
                }
                size_t write(uint8_t c){
                    return Serial.write(c);
                }

                size_t write(const uint8_t *buffer, size_t size){
                    return Serial.write(buffer,size);
                }
        };
        #else 
        class SerialEvoAppender: public EvoAppender {
            public:
                void begin(){
                    log_w("ArduinoEvoAppender start");
                }
                void end(){
                    log_w("ArduinolEvoAppender stop");
                }
                size_t write(uint8_t c){
                    log_w(c);
                    return 1;
                }

                size_t write(const uint8_t *buffer, size_t size){
                    log_w(buffer);
                    return size;
                }
        };
        #endif
    #endif

    class EvoDebug {
    private:
        EvoDebug(){
            #ifdef EVODEBUG_SERIAL
                EvoAppender *serialAppender = new SerialEvoAppender();
                addEvoAppender(serialAppender);
            #endif
        }

        std::list<EvoAppender*> appenders;

        ~EvoDebug(){
            std::list<EvoAppender*> :: iterator it; 
            for(it = appenders.begin(); it != appenders.end(); ++it) 
                 (*it)->end();
        }

        const char* LEVEL_NAME[5] = {"Debug","Info","Warning","Error"};

    public:
        EvoDebug(const EvoDebug&) = delete;
        EvoDebug& operator=(const EvoDebug &) = delete;
        EvoDebug(EvoDebug &&) = delete;
        EvoDebug & operator=(EvoDebug &&) = delete;

        static EvoDebug& instance(){
            static EvoDebug evoDebug;
            return evoDebug;
        }

        void addEvoAppender(EvoAppender *evoAppender){ appenders.push_back(evoAppender); evoAppender->begin(); }

        void message(EvoAppender::LEVEL level,const char *fileName,const char *functionName, uint line, const char *fmt, ...){
            va_list args;
            uint8_t p;
            std::list<EvoAppender*> :: iterator it; 
                for(it = appenders.begin(),p=0; it != appenders.end(); ++it,++p) { 
                    (*it)->printf("[%s] %s ",LEVEL_NAME[level],functionName);
                    va_start(args,fmt);
                    (*it)->vprintf(fmt,args);
                    va_end(args);
                    (*it)->printf(" [%s:%d]",fileName,line);
                    (*it)->println();
                }
        }
    }; 

    #define evoDebug EvoDebug::instance() 
    #define debugD(args...) EvoDebug::instance().message(EvoAppender::LEVEL::DEBUG,__FILE__ , __PRETTY_FUNCTION__ , __LINE__, args )
    #define debugI(args...) EvoDebug::instance().message(EvoAppender::LEVEL::INFO,__FILE__ , __PRETTY_FUNCTION__ , __LINE__, args )
    #define debugW(args...) EvoDebug::instance().message(EvoAppender::LEVEL::WARNING,__FILE__ , __PRETTY_FUNCTION__ , __LINE__, args )
    #define debugE(args...) EvoDebug::instance().message(EvoAppender::LEVEL::ERROR,__FILE__ , __PRETTY_FUNCTION__ , __LINE__, args )

#else
    #define debugD //
    #define debugI //
    #define debugW //
    #define debugE //
#endif 