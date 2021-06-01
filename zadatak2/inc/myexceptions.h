#include <string>

struct DuplicateSymbolException{
   DuplicateSymbolException(std::string msg) {
       msg_ = msg;
   }
  ~DuplicateSymbolException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};

struct UndefinedSymbolException{
   UndefinedSymbolException(std::string msg) {
       msg_ = msg;
   }
  ~UndefinedSymbolException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};


struct CantOpenFileException{
   CantOpenFileException(std::string msg) {
       msg_ = msg;
   }
  ~CantOpenFileException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};

struct NotElfFileException{
   NotElfFileException(std::string msg) {
       msg_ = msg;
   }
  ~NotElfFileException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};

struct NotRelocatableObjectFileException{
   NotRelocatableObjectFileException(std::string msg) {
       msg_ = msg;
   }
  ~NotRelocatableObjectFileException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};