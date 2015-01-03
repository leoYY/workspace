# -*- coding: UTF-8 -*-
"""
    @author     : yuanyi
    @date       : Sat 03 Jan 2015 09:03:21 AM UTC
    @last update: Sat 03 Jan 2015 09:03:21 AM UTC
    @summary    : 
    @version    : 1.0.0.0
"""

import sys,os,getopt,logging,traceback

# ================ log ===============
LOGFILE="./ext_closure_impl.log"

LOGLEVEL="NOTICE"
FORMAT="[%(levelname)s] %(asctime)s : %(pathname)s %(module)s:%(funcName)s[%(lineno)d] %(message)s"
LEVEL = {}
LEVEL['NOTICE'] = logging.NOTSET
LEVEL['DEBUG'] = logging.DEBUG
LEVEL['INFO'] = logging.INFO
LEVEL['WARNING'] = logging.WARNING
LEVEL['ERROR'] = logging.ERROR
LEVEL['CRITICAL'] = logging.CRITICAL

def InitLog():
    logger = logging.getLogger()
    hdlr = logging.FileHandler(LOGFILE)
    formatter = logging.Formatter(FORMAT)
    hdlr.setFormatter(formatter)
    logger.addHandler(hdlr)
    logger.setLevel(LEVEL[str(LOGLEVEL)])
    return logger

LOG=InitLog()

# =====================================

def create_ext_closure(f, prearg, postarg):
    f.write("// Copyright (c) 2015 Baidu.com , Inc. All Rights Reserved\n")
    f.write("// @author: yuanyi (yuan.yi.coding@gmail.com)\n")
    f.write("// @brief : \n\n #include \"noncopyable.h\n")
    f.write("#ifndef _EXT_CLOSURE_H_\n")
    f.write("#define _EXT_CLOSURE_H_\n\n")
    f.write("class ExtClosure : public noncopyable {\n")
    f.write("public:\n")
    f.write("    ExtClosure() {}\n")
    f.write("    virtual ~ExtClosure();\n")
    f.write("};\n\n")
    create_function(f, prearg, postarg)
    create_method(f, prearg, postarg)
    f.write("\n#endif\n")

def create_function(f, prearg, postarg):
    pre = prearg + 1
    post = postarg + 1
    f.write("template <") 
    for i in range(1, pre) :
        for j in range(1, post) :
            # implement template arg
            arg_count = 0 
            for p_arg in range(1, i+1):
                arg_count += 1 
                if arg_count != 1:
                    f.write(", typename PreArg%d"%(p_arg))
                else :
                    f.write("typename PreArg%d"%(p_arg))
            for pt_arg in range(1, j+1):
                f.write(", typename PostArg%d"%(pt_arg))
            f.write(">\n")
            # implement function declare
            f.write("class FunctionExtClosure%d_%d : public ExtClosure {\n"%(i, j))
            f.write("public:\n")
            f.write("    typedef void (*FunctionType)(")
            for p_arg in range(1, i+1):
                if p_arg != 1:
                    f.write(", PreArg%d"%(p_arg))
                else:
                    f.write("PreArg1")
            for pt_arg in range(1, j+1):
                f.write(", PostArg%d"%(pt_arg))
            f.write(");\n")
            f.write("};\n")


    pass

def create_method(f, prearg, postarg):
    pass

def usage():
    print "Usage: ext_closure_impl.py [options] ..."
    print "       --PreArgCount pre arg max count"
    print "       --PostArgCount post arg max count"

def main():
    try:
        opts,args = getopt.getopt(sys.argv[1:],"vh",["version","help", "PreArgCount=", "PostArgCount="])
    except GetoptError:
        sys.exit(2)

    PreArgCount = 3
    PostArgCount = 3

    for opt,val in opts:
        if opt == "h" :
            usage()
        elif opt in ('--PreArgCount') :         
            PreArgCount = int(val)
        elif opt in ('--PostArgCount') :
            PostArgCount = int(val)
    
    f = open("ext_closure.h", "w")
    create_ext_closure(f, PreArgCount, PostArgCount)

if __name__ == "__main__":
    try :
        main()
    except Exception, e:
        LOG.error("%s"%(traceback.print_exc()))
        sys.exit(-1)

