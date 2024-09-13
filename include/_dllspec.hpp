#ifndef _dllspec_hpp
#define _dllspec_hpp

#ifdef PARSER_API_EXPORTS
#define PARSER_API __declspec(dllexport)
#elif defined(PARSER_STATIC)
#define PARSER_API
#else
#define PARSER_API __declspec(dllimport)
#endif

class PARSER_API dbParser {
public:
    virtual bool parse(const std::string& filePath) = 0;
    virtual ~dbParser() {}
};

#endif /* _dllspec_hpp */