#include <iostream>

using namespace std;

template <class T>
class dequeue {
private:
  class queue_node {
  public:
    queue_node *prev;
    queue_node *next;
    T& data;
    // The stuff between the colon and the basic block is an "initialization 
    // list".  Code here gets executed before the method is called.
    queue_node (T &t) : prev(0), next(0), data(t) {}
  };

  queue_node *head, *tail;
  int size;

public:
  dequeue()
  {
    head = tail = 0;
    size = 0;
  }
  ~dequeue()
  {
    queue_node *q;

    for (q = head; q; q = head) {
      head = head->next;
      delete q;
    }
    tail = 0;
    size = 0;
  }
  bool is_empty() throw ()
  {
    return size == 0;
  }
  void insert_head(T& d) throw (const char *)
  {
    queue_node *q = new queue_node(d);

    if (!q) {
      throw "Failed to allocate storage for queue node";
    }

    if (head) {
      q->next = head;
      head->prev = q;
      head = q;
    } else {
      head = tail = q;
    }
    size++;
  }

  void insert_tail(T& d)
  {
    queue_node *q = new queue_node(d);

    if (!q) {
      throw "Failed to allocate storage for queue node";
    }

    if (tail) {
      q->prev = tail;
      tail->next = q;
      tail = q;
    } else {
      head = tail = q;
    }
    size++;
  }

  T& remove_head(void)
  {
    queue_node *q;

    if (!size) {
      throw "Attempt to pop an empty queue";
    }

    T& d = head->data;
    q = head;

    if (size > 1) {
      head = head->next;
      head->prev = 0;
    } else {
      head = tail = 0;
    }
    delete q;
    size--;

    return d;
  }

  T& remove_tail(void)
  {
    queue_node *q;

    if (!size) {
      throw "Attempt to pop an empty queue";
    }

    T& d = tail->data;
    q = tail;

    if (size > 1) {
      tail = tail->prev;
      tail->next = 0;
    } else {
      head = tail = 0;
    }
    delete q;
    size--;

    return d;
  }

  T& peek_head(void)
  {
    if (head) { // if (size) {   if (tail) {
      return head->data;
    }

    throw "Attempt to peek on an empty queue";
  }
  T& peek_tail(void)
  {
    if (head) { // if (size) {   if (tail) {
      return tail->data;
    }

    throw "Attempt to peek on an empty queue";
  }
};

int main(int argc, char *argv[])
{
  dequeue<int> d;
  dequeue<string> ds;

  int j = 2, k = 3, l = 5, m = 7, n = 11;

  d.insert_head(j);
  d.insert_head(k);
  d.insert_head(l);
  cout << "head is " << d.peek_head() << endl;
  cout << "tail is " << d.peek_tail() << endl;
  d.insert_head(m);
  d.insert_head(n);

  try {
    while (!d.is_empty()) {
      cout << d.remove_head() << endl;
    }
  }
  catch (...) {
    throw;
  }

  string s, t;

  s = "Hello";
  t = "World";

  ds.insert_tail(s);
  ds.insert_tail(t);

  while (!ds.is_empty()) {
    cout << ds.remove_tail() << endl;
  }

  return 0;
}
