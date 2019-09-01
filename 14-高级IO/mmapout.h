//
// Created by  sky on 2017/11/12.
//

#ifndef SLNODE_SLNODE_H
#define SLNODE_SLNODE_H

#endif //SLNODE_SLNODE_H


struct Node
{
    DataType data;
    Node *next;
    Node(const DataType &d):data(d), next(NULL)
    {

    }

    Node()
    {
        data = 0;
        next = NULL;        // of vital importance, in head insert list, ends the link list
    }
};

class SLinkList
{
private:
    Node *head;
    int size;

public:

    SLinkList()
    {
        head = new Node();      // with head node, also means deep copy
        size = 0;
    }

    virtual ~SLinkList()    // new list with one node, call this function error: SLNode(10166,0x7fffd367f3c0) malloc: *** error for object 0x7f8c7ec02600: pointer being freed was not allocated
    {
        Node *p = head;
        Node *q;

        while (p != NULL)
        {
            q = p;
            p = p->next;
            delete  q;//free(q);
        }
    }

    void create_list_head()
    {
        for (int i=0; i<10; ++i)
        {
            insert_pos(rand() % 100, 0);
        }
    }

    bool insert_pos(DataType x, int position)
    {
        Node *p = head;
        Node *q = NULL;

        int j = -1;

        // point to the location before position
        while (p->next != NULL && j < position - 1)
        {
            p = p->next;
            j++;
        }

        if (j != position - 1)
        {
            cout << "SLinkList insert position error." << endl;
            return false;
        }

        q = new Node(x);
        q->next = p->next;
        p->next = q;
        cout << "SLinkList insert data success." << endl;

        return true;
    }

    bool delete_pos(DataType &x, int position)
    {
        Node *p = head;
        Node *q = NULL;

        int j = -1;

        // point to the location before position, make sure the position not empty, has the real data
        while (p->next != NULL && p->next->next != NULL & j < position - 1)
        {
            p = p->next;
            j++;
        }

        if (j != position - 1)
        {
            cout << "SLinkList delete position error." << endl;
            return false;
        }

        q = p->next;
        x = q->data;
        p->next = p->next->next;
        delete(q);
        cout << "SLinkList delete data success." << endl;

        return true;
    }

    Node* get_node(int position)
    {
        Node *p = head;
        int j = -1;

        while (p->next != NULL && j < position)
        {
            p = p->next;
            j++;
        }

        if (j != position )
        {
            cout << "SLinkList get position error." << endl;
            return NULL;
        }

        return p;
    }

    int get_size()
    {
        Node *p = head;
        size = 0;

        while (p->next != NULL)
        {
            size++;
            p = p->next;
        }

        return size;
    }

    void print_linklist()
    {
        Node *p = head;

        while (p->next != NULL)
        {
            cout << p->next->data << '\t';
            p = p->next;
        }

        cout << endl;
    }

    void destroy_linklist(SLinkList list)
    {
        Node *p = list.head;
        Node *q = list.head;

        while (p != NULL)
        {
            q = p;
            p = p->next;
            delete  q;//free(q);
        }
    }


    // reverse the single link list
    bool reverse_linklist()
    {
        SLinkList new_list;
        Node *p = head;
        Node *temp;

        if (head->next == NULL)
        {
            cout << "the original link list is empty." << endl;
            return false;
        }

        new_list = SLinkList();

        if (new_list.head == NULL)
        {
            cout << "create the new link list failed." << endl;
            return false;
        }

        while (head->next != NULL)
        {
            temp = new_list.head->next;
            new_list.head->next = head->next;
            head->next = head->next->next;
            new_list.head->next->next = temp;
        }


        p->next = new_list.head->next;  // link to the original link list

        new_list.head->next = NULL;     // break to the new link list

        //delete new_list.head;

        return true;
    }

};