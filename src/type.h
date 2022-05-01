
#define ERIC_GEN_OTHER_TYPE(T)        \
	typedef const T* const T##_sp;    \
	typedef T* const T##_p;           \
	typedef T* T##_usp;               \
    typedef volatile T  v##T;         

#ifdef _UNICODE
typedef wchar_t  echar;
#define TCreateFile CreateFileW
#else
typedef char     echar;
#define TCreateFile CreateFile
#endif

ERIC_GEN_OTHER_TYPE(echar);

#define EXPORT_DLL __declspec(dllexport)