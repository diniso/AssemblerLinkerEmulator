#include <string>

class NoSuchRegisterException{

public:
   NoSuchRegisterException(std::string msg) {
       msg_ = msg;
   }
  ~NoSuchRegisterException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};


class NoSuchAddresTypeException{

public:
   NoSuchAddresTypeException(std::string msg) {
       msg_ = msg;
   }
  ~NoSuchAddresTypeException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};


class NoFlagsForInstructionException{

public:
   NoFlagsForInstructionException(std::string msg) {
       msg_ = msg;
   }
  ~NoFlagsForInstructionException() {}

   std::string getMessage() const {return(msg_);}
private:
   std::string msg_;
};