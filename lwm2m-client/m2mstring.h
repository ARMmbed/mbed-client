#include <sys/types.h> // size_t
#include <stdexcept>


namespace m2m
{

  /** @brief Simple C++ string class,used as replacement for
   std::string.
   */
  class String
  {
    char*     p;           ///< The data
    size_t    allocated_;  ///< The allocated memory size (including trailing NUL)
    size_t    size_;       ///< The currently used memory size (excluding trailing NUL)

  public:
    typedef size_t size_type;
    static const   size_type npos;

    String();
    ~String();
    String(const String&);
    String(const char*);

    String&  operator=(const char*);
    String&  operator=(const String&);

    String&  operator+=(const String&);
    String&  operator+=(const char*);
    String&  operator+=(char);
    void     push_back(char);

    friend String
    operator+(const String& lhs, const String& rhs);

    bool     operator==(const char*) const;
    bool     operator==(const String&) const;

    void     clear();       // set string to empty string (memory remains reserved)
    void     clearMemory(); // set string to empty string (memory is free'd)

    size_type size()   const   { return size_; }   ///< size without terminating NUL
    size_type length() const   { return size_; }   ///< as size()

    /// size if fully used
    size_type capacity() const { return allocated_-1; }

    bool      empty() const    { return size_ == 0; }

    const char*  c_str() const { return p; } ///< raw data

    /** Reserve internal string memory so that n characters can be put into the
        string (plus 1 for the NUL char). If there is already enough memory,
        nothing happens, if not, the memory will be realloated to exactly this
        amount.
        */
    void reserve( size_type n);

    /** Resize string. If n is less than the current size, the string will be truncated.
        If n is larger, then the memory will be reallocated to exactly this amount, and
        the additional characters will be NUL characters.
        */
    void resize( size_type n);

    /** Resize string. If n is less than the current size, the string will be truncated.
        If n is larger, then the memory will be reallocated to exactly this amount, and
        the additional characters will be c characters.
        */
    void resize( size_type n, char c);

    /// swap contents
    void swap( String& );

    String   substr(const size_type pos, size_type length) const;

    // unchecked access:
    char&    operator[](const size_type i)       { return p[i]; }
    char     operator[](const size_type i) const { return p[i]; }
    // checked access:
    char&    at(const size_type i);
    char     at(const size_type i) const;

    /// erase len characters at position pos
    String& erase(size_type pos, size_type len);
    /// Append n characters of a string
    String& append(const char* str, size_type n);

    int     compare( size_type pos, size_type len, const String& str ) const;
    int     compare( size_type pos, size_type len, const char*   str ) const;

    int     find_last_of(char c) const;

  private:
    // reallocate the internal memory
    void  new_realloc( size_type n);
    char* strdup_never_null(const char* other);

    char _return_value;

  };
  // class

  bool
  operator<(const String&, const String&);

} // namespace
