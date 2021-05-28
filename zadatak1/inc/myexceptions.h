#include <string>

class DuplicateSymbolException{

public:
   DuplicateSymbolException(std::string msg) {
       msg_ = msg;
   }
  ~DuplicateSymbolException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};

class GlobalSymbolException{

public:
   GlobalSymbolException(std::string msg) {
       msg_ = msg;
   }
  ~GlobalSymbolException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};