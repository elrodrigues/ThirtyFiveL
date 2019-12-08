#!/usr/bin/env python3
import os, sys, zlib

class CommitNode:
    def __init__(self, commit_hash):
        """
        :type commit_hash: str
        """
        self.commit_hash = commit_hash
        self.parents = set()
        self.children = set()

def dfsStack(start_hash, cwd):
    start_vertex = CommitNode(start_hash)
    stack = [start_vertex]
    visited = []
    v_hashes = set()
    while stack:
        v = stack.pop()
        visited.append(v)
        v_hashes.add(v.commit_hash)
        objpath = cwd+v.commit_hash[0]+v.commit_hash[1]
        # print(objpath)
        if not os.path.isdir(objpath):
            sys.exit('Unexpected error')
        os.chdir(objpath)
        new_hashes = []
        if os.access(v.commit_hash[2:], os.R_OK):
            with open(v.commit_hash[2:], 'rb') as fp:
                new_log = zlib.decompress(fp.read())
                new_hashes = new_log.decode('utf8').split("\n")
        pin = -1
        for i in range(1, len(new_hashes)):
            token = new_hashes[i]
            if not 'parent' in (token.split(' ')[0]):
                pin = i
                break
        if pin > 1:
            new_hashes = new_hashes[1:pin]
            parHash = [s[7:] for s in new_hashes]
            # print(parHash)
            # print(v.commit_hash)
            for par in parHash:
                v.parents.add(par)
                if par not in v_hashes:
                    nextNode = CommitNode(par)                   
                    nextNode.children.add(v.commit_hash)
                    stack.append(nextNode)
                else: # Fix broken link
                    for q in visited:
                        if q.commit_hash == par:
                            q.children.add(v.commit_hash)
                            break;
    return visited

sorted_paths = []
perm_mark = {}

def topos_sort(roots, commits, dic):
    global perm_mark
    for r in roots:
        if r not in perm_mark:
            visit(r, commits, dic)
    return


def visit(r, commits, dic):
    global perm_mark
    global sorted_paths
    if r in perm_mark:
        return
    for ch in sorted(list(r.children)):
        indx = dic[ch]
        ch_v = commits[indx]
        visit(ch_v, commits, dic)
    perm_mark[r] = 0
    sorted_paths.append(r)
    
        
def main():
    global sorted_paths
    # Step 1
    cwd = os.getcwd()
    flag = True
    while os.getcwd() != '/':
        if not os.path.isdir(cwd+"/.git"):
            cwd = cwd+"/.."
            os.chdir(cwd)
        else:
            flag = False
            break
    if flag:
        sys.exit('Not Inside a Git repository')
    # Step 2
    cwd=cwd+"/.git"
    refpath = cwd+"/refs/heads/"
    if not os.path.isdir(refpath):
        sys.exit('Unexpected error')
    os.chdir(refpath)
    cont_dir = os.listdir(refpath) # branches
    sha_hash = [] # respective hashes
    for branch in cont_dir:
        if os.access(branch, os.R_OK):
            with open(branch, 'r') as fp:
                sha_hash.append(fp.read()[:-1])
    print(cont_dir)
    # Step 3
    if sha_hash:
        paths = [dfsStack(h, cwd+"/objects/") for h in sha_hash]
        # Union
        mergedHashes = {} # Dict: hash -> index in union
        branchDict = {} # Dict: hash -> set{branch names}
        commit_tree = [] # Union tree
        count = 0
        for p in paths:
            l = len(p)
            for i in range(0, l):
                c = p[i]
                if c.commit_hash not in mergedHashes:
                    mergedHashes[c.commit_hash] = count
                    branchDict[c.commit_hash] = set()
                    commit_tree.append(c)
                    count = count + 1
                else:
                    ind = mergedHashes[c.commit_hash]
                    commit_tree[ind].parents = commit_tree[ind].parents.union(c.parents)
                    commit_tree[ind].children = commit_tree[ind].children.union(c.children)
        l = len(sha_hash)
        for i in range(0, l):
            branchDict[sha_hash[i]].add(cont_dir[i])
        # [print(p.commit_hash, " : ", p.parents) for p in commit_tree]
        # print(branchDict['777e41d9a9ba12b9a485300e16fa4405a4f633c0'])
    else:
        sys.exit("No branches found")        
    # Step 4
    root_commits = set()
    for c in commit_tree:
        if c.parents == set():
            root_commits.add(c)
    topos_sort(root_commits, commit_tree, mergedHashes)

    # [print(sp.commit_hash) for sp in sorted_paths] # seems to be working
    # Step 5
    prev = []
    output = ''
    for sp in sorted_paths:
        if prev != []:
            pp = prev[0]
            if pp.commit_hash not in sp.children:
                output = list(output)
                output[-1] = '='
                output[-2] = ''
                output.append('\n')
                output.append('\n')
                output = "".join(output)
            output=output+sp.commit_hash+' '
            for nm in sorted(list(branchDict[sp.commit_hash])):
                output = output + nm + ' '
            output=output + '\n'
            prev = [sp]
        else:
            output=output+sp.commit_hash+' '
            for nm in sorted(list(branchDict[sp.commit_hash])):
                output = output + nm + ' '
            output=output + '\n'
            prev = [sp]
    print(output)
    
if __name__ == "__main__":
    main()
