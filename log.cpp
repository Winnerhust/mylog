#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

class LogModule;
class LogFormater;

#define log_debug(log,file,msg,userText) log.format(file,msg,LOG_DEBUG,userText,__FILE__,__LINE__)
#define log_info(log,file,msg,userText) log.format(file,msg,LOG_INFO,userText,__FILE__,__LINE__)
#define log_warning(log,file,msg,userText) log.format(file,msg,LOG_WARNING,userText,__FILE__,__LINE__)
#define log_error(log,file,msg,userText) log.format(file,msg,LOG_ERROR,userText,__FILE__,__LINE__)

enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

class MyLog
{

};

class LogModule
{
public:
    LogModule() {
        strncpy(m_fmt, "[%TIME][%LEVEL]", sizeof(m_fmt));
        m_userText[0] = '\0';
        m_filePath[0] = '\0';

        m_file = stdout;
        m_level = 1;
    }

    char *getModuleText() {
        return m_userText;
    }
    FILE *getFile() {
        return m_file;
    }
    char *getFilePath() {
        return m_filePath;
    }

    char *getFmt() {
        return m_fmt;
    }
    int getLevel() {
        return m_level;
    }

    void setModuleText(const char *userText) {
        strncpy(m_userText, userText, sizeof(m_userText));
    }
    void setFile(FILE *file) {
        m_file = file;
    }
    void setFilePath(const char *path) {
        strncpy(m_filePath, path, sizeof(m_filePath));
    }
    void setLevel(int level) {
        m_level = level;
    };
    void setFmt(const char *fmt) {

        strncpy(m_fmt, fmt, sizeof(m_fmt));
    }
private:
    char m_userText[256];
    char m_filePath[256];
    char m_fmt[256];
    FILE *m_file;
    int  m_level;
};

class LogFormater
{
public:
    /*    char *getTimeText();
        char *getFileText();
        char *getUserText();
        char *getLineText();
        char *getPidText();
        char *getLevelText();
    */
    char *getText(const char *fmt, char *buf, int len);

    void setFormat(const char *fmt);
    void setLevel(int level);
    void setUserText(const char *userText);
    void setFileText(const char *fileText);
    void setLine(int line);

    void format(FILE *file, const char *msg);
    void format(FILE *file, const char *msg, int level, const char *userText, const char *fileText, int line);

    LogFormater();
    ~LogFormater() {}
private:
    char m_fmt[256];
    char m_userText[256];
    char m_fileText[256];
    int  m_level;
    int  m_line;

    static const char fmtList[][32];
    static const int fmtNum;
    static const char levelText[][32];
};
const char LogFormater::fmtList[][32] = {
    "%TIME",
    "%LEVEL",
    "%USERTEXT",
    "%FILE",
    "%LINE",
    "%PID",
    "%%"
};
const int LogFormater::fmtNum = 7;

const char LogFormater::levelText[][32] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR"
};

/* %TIME%|%PID%|%LEVEL%|%MARKER%|%CAT%|%FILE%|%LINE%|%FUNC%|%USERTEXT%| */

LogFormater::LogFormater()
    : m_level(1),
      m_line(0)
{
    strncpy(m_fmt, "[%TIME][%LEVEL]", sizeof(m_fmt));
    m_userText[0] = '\0';
    m_fileText[0] = '\0';
}

void LogFormater::setFormat(const char *fmt)
{
    strncpy(m_fmt, fmt, sizeof(m_fmt));
    return;
}

void LogFormater::setUserText(const char *userText)
{
    strncpy(m_userText, userText, sizeof(m_userText));
    return;
}

void LogFormater::setLevel(int level)
{
    m_level = level;
    return;
}

void LogFormater::setFileText(const char *fileText)
{
    strncpy(m_fileText, fileText, sizeof(m_fileText));
    return;
}

void LogFormater::setLine(int line)
{
    m_line = line;
    return;
}

void LogFormater::format(FILE *file, const char *msg)
{
    char *p = m_fmt;
    char buf[256];
    bool isFmt;

    while (*p != '\0') {
        isFmt = false;

        if (*p == '%') {
            for (int i = 0; i < fmtNum; ++i) {
                if (strncmp(p, fmtList[i], strlen(fmtList[i])) == 0) {
                    fprintf(file, "%s", getText(fmtList[i], buf, sizeof(buf)));
                    p += strlen(fmtList[i]);
                    isFmt = true;
                    break;
                }
            }
        }

        if (!isFmt) {
            fprintf(file, "%c", *p);
            ++p;
        }
    }

    fprintf(file, "%s\n", msg);

    return;
}
void LogFormater::format(FILE *file, const char *msg, int level, const char *userText, const char *fileText, int line)
{
    setLevel(level);
    setUserText(userText);
    setFileText(fileText);
    setLine(line);

    return format(file, msg);
}

char *LogFormater::getText(const char *fmt, char *buf, int len)
{
    if (strncmp(fmt, "%%", strlen("%%")) == 0) {
        strncpy(buf, "%", len);
    } else if (strncmp(fmt, "%LEVEL", strlen("%LEVEL")) == 0) {
        strncpy(buf, levelText[m_level], len);
    } else if (strncmp(fmt, "%FILE", strlen("%FILE")) == 0) {
        strncpy(buf, m_fileText, len);
    } else if (strncmp(fmt, "%LINE", strlen("%LINE")) == 0) {
        snprintf(buf, len, "%d", m_line);
    } else if (strncmp(fmt, "%TIME", strlen("%TIME")) == 0) {
        time_t now = time(NULL);
        strftime(buf, len, "%Y-%m-%d %I:%M:%S", localtime(&now));
    } else if (strncmp(fmt, "%PID", strlen("%PID")) == 0) {
        snprintf(buf, len, "%d", getpid());
    } else if (strncmp(fmt, "%USERTEXT", strlen("%USERTEXT")) == 0) {
        snprintf(buf, len, "%s", m_userText);
    } else {
        buf[0] = '\0';
    }

    return buf;
}

int main()
{
    LogFormater log;
    log.setFormat("[%TIME][%LEVEL][%USERTEXT][%PID][%FILE:%LINE]%%");

    log.setLevel(2);
    log.setUserText("MAIN");
    log.setFileText("main.cpp");
    log.setLine(168);

    log.format(stdout, "Hello,world");

    log_info(log, stdout, "sdsfsdfs", "MAIN");
    log_error(log, stdout, "sdsfsdfs", "MAIN");
    return 0;
}
