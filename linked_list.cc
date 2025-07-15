#include "linked_list.h"

#include <cassert>

// Initial declaration of the static member function
// pointers in the linked_list class.
//
void* (*linked_list::malloc_fptr)(size_t) = nullptr;
void (*linked_list::free_fptr)(void*) = nullptr;

linked_list::linked_list() {}

linked_list::~linked_list()
{
    node* currentNode = head;
    for (size_t i = 0u; i < ll_size; ++i)
    {
        node* nextNode = currentNode->next;
        delete (currentNode);
        currentNode = nextNode;
    }
}

void* linked_list::operator new(size_t size) { return malloc_fptr(size); }

void linked_list::operator delete(void* ptr) { free_fptr(ptr); }

bool linked_list::insert_into_empty_list(unsigned int data)
{
    head = create_node(data, nullptr);
    if (head == nullptr) [[unlikely]]
    {
        return false;
    }

    return true;
}

linked_list::node* linked_list::create_node(unsigned int data, node* next)
{
    node* newNode = new node();
    if (newNode == nullptr) [[unlikely]]
    {
        return nullptr;
    }

    newNode->data = data;
    newNode->next = next;
    ++ll_size;
    return newNode;
}

bool linked_list::insert(size_t index, unsigned int data)
{
    if (index > ll_size) [[unlikely]]
    {
        return false;
    }

    if (is_empty())
    {
        return insert_into_empty_list(data);
    }

    if (index == ll_size)
    {
        return insert_end(data);
    }

    if (index == 0)
    {
        return insert_front(data);
    }

    // Find the node immediately before the desired index
    node* prevNode = head;
    for (size_t i = 1u; i < index; ++i)
    {
        prevNode = prevNode->next;
        assert(prevNode != nullptr);
    }

    node* newNode = create_node(data, prevNode->next);
    prevNode->next = newNode;

    return true;
}

bool linked_list::insert_front(unsigned int data)
{
    node* newNode = create_node(data, head);
    if (newNode == nullptr) [[unlikely]]
    {
        return false;
    }

    head = newNode;

    return true;
}

bool linked_list::insert_end(unsigned int data)
{
    if (is_empty())
    {
        return insert_into_empty_list(data);
    }

    node* newNode = create_node(data, nullptr);
    if (newNode == nullptr) [[unlikely]]
    {
        return false;
    }

    node* currentNode = head;
    for (size_t i = 2; i < ll_size; ++i)
    {
        currentNode = currentNode->next;
    }

    currentNode->next = newNode;

    return true;
}

size_t linked_list::find(unsigned int data) const
{
    node* currentNode = head;
    for (size_t i = 0u; i < ll_size; ++i)
    {
        if (currentNode->data == data)
            return i;

        currentNode = currentNode->next;
    }

    return SIZE_MAX;
}

bool linked_list::remove(size_t index)
{
    // Find the node immediately before the desired index.
    // Then orphan the desired node by linking prevNode -> nextNode
    // And then cleanup the orphaned node.

    if (is_empty() || index >= ll_size) [[unlikely]]
    {
        return false;
    }

    if (index == 0)
    {
        remove_head();
        return true;
    }

    node* prevNode = head;
    for (size_t i = 1u; i < index; ++i)
    {
        prevNode = prevNode->next;
    }

    node* currentNode = prevNode->next;

    // orphan the currentNode
    prevNode->next = currentNode->next;

    // cleanup
    --ll_size;
    delete (currentNode);

    return true;
}

void linked_list::remove_head()
{
    node* newHead = head->next;
    delete (head);
    head = newHead;
    --ll_size;
}

size_t linked_list::size() const { return ll_size; }

unsigned int& linked_list::operator[](size_t idx)
{
    node* currentNode = head;
    for (size_t i = 0; i < idx; ++i)
    {
        currentNode = currentNode->next;
    }

    return currentNode->data;
}

const unsigned int& linked_list::operator[](size_t idx) const
{
    node* currentNode = head;
    for (size_t i = 0; i < idx; ++i)
    {
        currentNode = currentNode->next;
    }

    return currentNode->data;
}

void linked_list::register_malloc(void* (*malloc)(size_t))
{
    linked_list::malloc_fptr = malloc;
}

void linked_list::register_free(void (*free)(void*))
{
    linked_list::free_fptr = free;
}

void* linked_list::node::operator new(size_t size)
{
    assert(linked_list::malloc_fptr);
    void* ptr = linked_list::malloc_fptr(size);

    return ptr;
}

void linked_list::node::operator delete(void* ptr)
{
    assert(linked_list::free_fptr);
    return linked_list::free_fptr(ptr);
}

bool linked_list::is_empty() const
{
    assert((head == nullptr) == (ll_size == 0u));
    return head == nullptr;
}
