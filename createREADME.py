# -*- coding: utf-8 -*- 
import os 
import os.path as path

def main ():
    # file_name(os.getcwd())
    # file = "E:\github_repo\CodingPractice\hosted_network\service_opt\service_opt.pro"
    # file = "E:\github_repo\CodingPractice\hosted_network\service_opt"
    # testRelpath(file)
    # level 0
    nodeList = []
    baseDir = "E:\github_repo\CodingPractice\CppPrimer"
    
    
    
    # createMD(nodeList, baseDir)
    # for aNode in nodeList:
        # print(aNode)
    
    toc = []
    dir = "E:\\github_repo\\CodingPractice\\CppPrimer\\chapter_12\\build-Debug"
    # dir = "E:/github_repo/CodingPractice/CppPrimer/chapter_12/build-Debug"
    # add2Toc(toc, dir)
    recurMD(toc, os.getcwd(), 1)

    with open('README.md', 'wt') as f:
        for one in toc: 
            one = one.replace("\\", "/") # github 的路径用 / 分隔
            f.write(one + "\n")
            # print(one)
            
    
    
    
    # toc.append("myNotes")
    # toc.append("---")
    
    pass


def recurMD(toc, baseDir, level=1):
    relPath = ""
    link = ""
    if level > 1: 
        relPath = baseDir.split("\\")
        toc.append("#" * level + " " + baseDir.split("\\")[-1])
        # relPath = path.relpath(baseDir)
        # toc.append("#" * level + " " + str(level) + " " + relPath)
    nodeList = os.listdir(baseDir)
    forwardDir = []
    for aNode in nodeList: 
        fullPath = path.join(baseDir, aNode)
        if path.isfile(fullPath):
            relPath = path.relpath(fullPath)
            link = "- [" + aNode + "]" + \
                "(" + relPath +")"
            toc.append(link)
        elif aNode == ".git": 
            continue
        else : 
            forwardDir.append(fullPath)
    
    for aDir in forwardDir: 
        recurMD(toc, aDir, level+1)
    return

def createMD(toc, baseDir, level=1): 
    # toc = []    # line per item
    stack = []
    # 
    stack.append(baseDir)
    while len(stack) > 0: 
        # node = stack[-1]
        node = stack.pop()
        toc.append(node)
        nodeList = os.listdir(node)
        forwardDir = []
        for aNode in nodeList: 
            fullPath = path.join(node, aNode)
            if path.isfile(fullPath): 
                toc.append(fullPath)
            else : 
                forwardDir.append(fullPath)
        stack.extend(reversed(forwardDir)) # 遍历需要反字母序压栈
    return

# 
# def file_name(file_dir):   
    # for root, dirs, files in os.walk(file_dir):  
        # print(root) #当前目录路径
        # print(files) #当前路径下所有非目录子文件
        # print(dirs) #当前路径下所有子目录
        # print()
    # return

# 相对路径
def testRelpath(file): 
    ppp = os.path.relpath(file) # 相对.py 的路径
    print(ppp)
    return

def add2Toc(toc, nod): 
    print(nod)
    relPath = ""
    if path.isdir(nod):
        relPath = path.relpath(nod)
    else :
        print("not dir")
    print(path.relpath(nod))
    relPath.replace("\\", "/")
    cnt = relPath.count("/")
    print(relPath)
    toc.append("#"*(1+cnt) + " " + relPath)
    print(toc)
    return

if __name__=="__main__":
    main()

# 先序遍历
# <http://blog.csdn.net/hyperbolechi/article/details/42913061>
# python3-cookbook
# <http://python3-cookbook.readthedocs.io/zh_CN/latest/c05/p01_read_write_text_data.html>