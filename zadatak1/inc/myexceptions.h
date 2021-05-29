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

class UndefinedSymbolException{

public:
   UndefinedSymbolException(std::string msg) {
       msg_ = msg;
   }
  ~UndefinedSymbolException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};

class NoSuchInstructionException{
public:
   NoSuchInstructionException(std::string msg) {
       msg_ = msg;
   }
  ~NoSuchInstructionException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};

class NoSuchAdressTypeException{
public:
   NoSuchAdressTypeException(std::string msg) {
       msg_ = msg;
   }
  ~NoSuchAdressTypeException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};

class CantCalculateInstrunctionLenghtException{
public:
   CantCalculateInstrunctionLenghtException(std::string msg) {
       msg_ = msg;
   }
  ~CantCalculateInstrunctionLenghtException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};