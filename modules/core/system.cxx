#include "precomp.hxx"
#include <iostream>

#include "openHL/core/utils/logger.hxx"

namespace hl
{
Exception::Exception()
{
    code = 0;
    line = 0;
}

Exception::Exception(int _code, const String& _err, const String& _func, const String& _file, int _line):
    code(_code), err(_err), func(_func), file(_file), line(_line)
{
    formatMessage();
}

Exception::~Exception() throw() {}

const char* Exception::what() const throw() { return msg.c_str(); }

void Exception::formatMessage()
{
    size_t pos       = err.find('\n');
    bool   multiline = pos != hl::String::npos;
    if (multiline)
    {
        std::stringstream ss;
        size_t            prev_pos = 0;
        while (pos != hl::String::npos)
        {
            ss << "> " << err.substr(prev_pos, pos - prev_pos) << std::endl;
            prev_pos = pos + 1;
            pos      = err.find('\n', prev_pos);
        }
        ss << "> " << err.substr(prev_pos);
        if (err[err.size() - 1] != '\n')
            ss << std::endl;
        err = ss.str();
    }
    if (func.size() > 0)
    {
        if (multiline)
            msg = std::format("OpenHL {}:{}: error: ({}:{}) in function '{}'\n{}", file.c_str(), line, code, hlErrorStr(code), func.c_str(), err.c_str());
        else
            msg = std::format("OpenHL {}:{}: error: ({}:{}) {} in function '{}'\n", file.c_str(), line, code, hlErrorStr(code), err.c_str(), func.c_str());
    }
    else
    {
        msg = std::format("OpenHL {}:{}: error: ({}:{}) {}{}", file.c_str(), line, code, hlErrorStr(code), err.c_str(), multiline ? "" : "\n");
    }
}

extern "C" const char* hlErrorStr(int status)
{
    static char buf[256];

    switch (status)
    {
        case HL_StsOk                  : return "No Error";
        case HL_StsBackTrace           : return "Backtrace";
        case HL_StsError               : return "Unspecified error";
        case HL_StsInternal            : return "Internal error";
        case HL_StsNoMem               : return "Insufficient memory";
        case HL_StsBadArg              : return "Bad argument";
        case HL_StsNoConv              : return "Iterations do not converge";
        case HL_StsAutoTrace           : return "Autotrace call";
        case HL_StsBadSize             : return "Incorrect size of input array";
        case HL_StsNullPtr             : return "Null pointer";
        case HL_StsDivByZero           : return "Division by zero occurred";
        case HL_BadStep                : return "Image step is wrong";
        case HL_StsInplaceNotSupported : return "Inplace operation is not supported";
        case HL_StsObjectNotFound      : return "Requested object was not found";
        case HL_BadDepth               : return "Input image depth is not supported by function";
        case HL_StsUnmatchedFormats    : return "Formats of input arguments do not match";
        case HL_StsUnmatchedSizes      : return "Sizes of input arguments do not match";
        case HL_StsOutOfRange          : return "One of the arguments\' values is out of range";
        case HL_StsUnsupportedFormat   : return "Unsupported format or combination of formats";
        case HL_BadCOI                 : return "Input COI is not supported";
        case HL_BadNumChannels         : return "Bad number of channels";
        case HL_StsBadFlag             : return "Bad flag (parameter or structure field)";
        case HL_StsBadPoint            : return "Bad parameter of type CvPoint";
        case HL_StsBadMask             : return "Bad type of mask argument";
        case HL_StsParseError          : return "Parsing error";
        case HL_StsNotImplemented      : return "The function/feature is not implemented";
        case HL_StsBadMemBlock         : return "Memory block has been corrupted";
        case HL_StsAssert              : return "Assertion failed";
        case HL_GpuNotSupported        : return "No CUDA support";
        case HL_GpuApiCallError        : return "Gpu API call";
        case HL_OpenGlNotSupported     : return "No OpenGL support";
        case HL_OpenGlApiCallError     : return "OpenGL API call";
    };

    snprintf(buf, sizeof(buf), "Unknown %s code %d", status >= 0 ? "status" : "error", status);
    return buf;
}

void error(const Exception& exc)
{
    throw exc;
}

void error(int _code, const String& _err, const char* _func, const char* _file, int _line)
{
    error(hl::Exception(_code, _err, _func, _file, _line));
}

}    // namespace hl