#ifndef NDB_H
#define NDB_H
#include <nnode.h>

class DB
{
    Node<nString>* dnode;
    
public:
    DB(Node<nString>* node)
    {
        dnode = node;
    }
    void get(nString table, nString query, Remote<nString>::whenHandler handler)
    {
        nString v = "db get " + table + " " + query + " " + dnode->address;
        auto& remote = dnode->get_node<nString>("db");
        remote.set(v);
        remote.when(table, handler);
    }
    void set(nString table, nString val)
    {
        nString v = "db set " + table + " " + val + " " + dnode->address;
        auto& remote = dnode->get_node<nString>("db");
        remote.set(v);
    }
    void drop(nString table)
    {
        nString v = "db set " + table + " drop " + dnode->address;
        auto& remote = dnode->get_node<nString>("db");
        remote.set(v);
    }
    void remove(nString table, nString query)
    {
        nString v = "db set " + table + " remove " + query + " " + dnode->address;
        auto& remote = dnode->get_node<nString>("db");
        remote.set(v);
    }
    void aggregate(nString table, nString query, Remote<nString>::whenHandler handler)
    {
        nString v = "db get " + table + " aggregate " + query + " " + dnode->address;
        auto& remote = dnode->get_node<nString>("db");
        remote.set(v);
        remote.when(table, handler);
    }
};

#endif