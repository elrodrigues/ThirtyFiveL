#!/usr/bin/env python3
import os, sys, zlib

class CommitNode:
    def __init__(self, c_hash):
        """
        :type c_hash: str
        """
        self.c_hash = c_hash
        self.par = set()
        self.chl = set()

def construct_commit_tree(b_hash, tree, cwd):
    if b_hash in tree:
        return tree
    start_vertex = CommitNode(b_hash)
    stack = [start_vertex]
    v_hashes = set()

    while stack: # while not empty
        cur_v = stack.pop()
        cur_hash = cur_v.c_hash
        if cur_hash in v_hashes:
            continue
        v_hashes.add(cur_hash)
        path = cwd+cur_hash[0:2]
        os.chdir(path)
        
        new_hashes = []
        with open(cur_hash[2:], 'rb') as fp:
            new_log = zlib.decompress(fp.read()).decode().split("\n")

        pin = -1
        ln = len(new_log)
        for i in range(1, ln):
            token = new_log[i]
            if 'parent' not in token:
                pin = i
                break

        if pin > 1: # for multiple parents
            new_hashes = new_log[1:pin]
            par_hashes = [n_h.split(' ')[1] for n_h in new_hashes]
            
            for P in par_hashes:
                cur_v.par.add(P)
                if P in tree:
                    next_v = tree[P]
                    next_v.chl.add(cur_hash)
                    tree[cur_hash] = cur_v
                else:
                    next_v = CommitNode(P)
                    next_v.chl.add(cur_hash)
                    tree[cur_hash] = cur_v
                    tree[P] = next_v
                    stack.append(next_v)
                            
    return tree

def topo_sort(tree, roots):
    visited_hashes = set()
    sorted_commits = []
    for r in roots: # cycle through roots
        if r not in visited_hashes: # if not visited
            # Perform DFS
            grey_stack = [r] # temporary mark
            black_stack = [r] # perma-death
            local_sort = [] # locally sorted commits
            while grey_stack:
                count_chl = 0 # count children
                cur_hash = grey_stack.pop()
                cur_v = tree[cur_hash]
                visited_hashes.add(cur_hash)
                
                for C in sorted(list(cur_v.chl)):
                    if C not in visited_hashes:
                        grey_stack.append(C)
                        black_stack.append(C)
                        count_chl = count_chl + 1
                if count_chl == 0 and black_stack != []: # redundant precautions
                    if black_stack[-1] not in local_sort:
                        local_sort.append(black_stack.pop()) # add to list
                    while black_stack != []:
                        if black_stack[-1] not in local_sort:
                            break
                        else:
                            black_stack.pop()
                    if black_stack != []:
                        grey_stack.append(black_stack[-1])

        sorted_commits = sorted_commits + local_sort
            
    return sorted_commits
        
def topo_order_commits():
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
        sys.exit('Not inside a Git repository')

    cwd = cwd + "/.git"
    ref_path = cwd + "/refs/heads/" # ref path
    if not os.path.isdir(ref_path):
        sys.exit('Missing references in .git')
    os.chdir(ref_path)
    branches = sorted(list(os.listdir(ref_path))) # branches
    if not branches:
        sys.exit('No branches found')
    b_hash = [] # branch head hashes
    for b in branches:
        with open(b, 'r') as fp:
            b_hash.append(fp.read()[:-1])

    c_tree = {} # commit tree
    obj_path = cwd + '/objects/'
    for b_h in b_hash:
        c_tree = construct_commit_tree(b_h, c_tree, obj_path)

    # for key_hash in c_tree:
    #     print('key', key_hash)
    #     print('stored', c_tree[key_hash].c_hash)
    #     print('parent', c_tree[key_hash].par)
    #     print('child', c_tree[key_hash].chl)
    #     print('======')
    # print('Length:', len(c_tree))

    root_commits = set()
    for c in c_tree:
        if c_tree[c].par == set():
            root_commits.add(c)
    sorted_c = topo_sort(c_tree, root_commits)

    # for sor_h in sorted_c:
    #     print(sor_h)
    #     print('stored', c_tree[sor_h].c_hash)
    #     print('parent', c_tree[sor_h].par)
    #     print('child', c_tree[sor_h].chl)
    #     print('======')

    ln = len(b_hash)
    h_2_b = {} # branch hash -> branch
    for i in range(0, ln):
        if b_hash[i] not in h_2_b:
            h_2_b[b_hash[i]] = set()
        h_2_b[b_hash[i]].add(branches[i])
                             
    ln = len(sorted_c)
    i = 0
    output = ''
    while i < ln: # Edit
        sor_h = sorted_c[i] # current hash to output
        output = output + sor_h # final output
        if sor_h in b_hash:
            for b_name in sorted(list(h_2_b[sor_h])):
                output = output + ' ' + b_name
        if (i+1) < ln:
            next_sor_h = sorted_c[i+1]
            if next_sor_h not in c_tree[sor_h].par:
                nylon = len(c_tree[sor_h].par)
                ct = 0
                output = output + '\n'
                for P in sorted(list(c_tree[sor_h].par)):
                    output = output + P
                    ct = ct + 1
                    if ct < nylon:
                        output = output + ' '
                output = output+'=\n\n='
                nylon = len(c_tree[next_sor_h].chl)
                ct = 0
                for C in sorted(list(c_tree[next_sor_h].chl)):
                    output = output + C
                    ct = ct + 1
                    if ct < nylon: # elif
                        output = output + ' '
                    
        output = output + '\n'
        i = i + 1
    print(output)
            
if __name__ == "__main__":
    topo_order_commits()
