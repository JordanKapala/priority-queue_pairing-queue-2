// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C42CF4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{ nullptr }
            {}

            // Description: Allows access to the element at that Node's
            // position.  There are two versions, getElt() and a dereference
            // operator, use whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data
            // members of this Node class from within the PairingPQ class.
            // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
            // function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;

            // Add one extra pointer (parent or previous) as desired.
    }; // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{ nullptr }, num_nodes{ 0 } {
        // TODO: Implement this function.
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, root{ nullptr }, num_nodes{ 0 } {
        while (start != end) {
            push(*start);
            start++;
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare }, root{ nullptr }, num_nodes{ 0 } {
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid pairing heap.
        std::deque<Node*> dq;
        dq.push_back(other.root);

        while (!dq.empty()) {
            Node * cp = dq.front();
            dq.pop_front();
            if (cp->child) {
                dq.push_front(cp->child);
            }
            if (cp->sibling) {
                dq.push_front(cp->sibling);
            }
            push(cp->getElt());
        }
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        // lecture.
        PairingPQ temp(rhs);

        std::swap(num_nodes, temp.num_nodes);
        std::swap(root, temp.root); //root points to temp (rhs) root
        return *this; //og heap gets deconstructed in the form of temp call decon.
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        if (root) {
            std::deque<Node*> dq;
            dq.push_back(root);
            while (!dq.empty()) {
                Node * cp = dq.front();
                dq.pop_front();
                if (cp->child) {
                    dq.push_front(cp->child);
                }
                if (cp->sibling) {
                    dq.push_front(cp->sibling);
                }
                delete cp;
            }
        }
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        std::deque<Node*> dq;
        dq.push_back(root);
        root = nullptr;
        while (!dq.empty()) {
            Node * cp = dq.front();
            dq.pop_front();
            if (cp->child) {
                dq.push_front(cp->child);
            }
            if (cp->sibling) {
                dq.push_front(cp->sibling);
            }
            cp->child   = nullptr;
            cp->sibling = nullptr;
            cp->parent  = nullptr;
            root = meld(root, cp);
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        Node * p = root->child;
        delete root;
        --num_nodes;

        if (num_nodes == 0) {
            root = nullptr;
            return ;
        }

        std::deque<Node*> dq;
        for (Node * s = p; s != nullptr; s = s->sibling) {
            dq.push_back(s);
        }

        while (dq.size() > 1) {
            Node * p1 = dq[0];
            Node * p2 = dq[1];

            p1->sibling = nullptr; p1->parent  = nullptr;
            p2->sibling = nullptr; p2->parent  = nullptr;

            dq.push_back(meld(p1,p2));
            dq.pop_front();
            dq.pop_front();
        }

        root = dq[0]; //get new root

    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return root->getElt();
    } // top()

    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return num_nodes;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return root == nullptr;
    } // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        node->elt = new_value;

        if (this->compare(node->parent->getElt(), new_value)) { //possibly not needed?
            Node * p = node->parent->child;
            if (p == node) {
                node->parent->child   = node->sibling;
                node->sibling         = nullptr;
                node->parent          = nullptr;
                root = meld(root, node);
            }
            else {
                while (p->sibling != node) {
                    p = p->sibling;
                }
                p->sibling = node->sibling;
                node->sibling = nullptr;
                node->parent  = nullptr;
                root = meld(root, node);
            }
        }
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node* addNode(const TYPE &val) {
        num_nodes++;
        Node * new_node = new Node(val);
        root = meld(root, new_node);
        return new_node;
    } // addNode()


private:
    // TODO: Add any additional member variables or member functions you
    // require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    // papers).

    Node * meld(Node *pa, Node *pb) {
        if (!pa) {
            return pb;
        }
        if (!pb) {
            return pa;
        }

        if (this->compare(pa->elt, pb->elt)) { //root1 < root2
            //pa is pb's child
            //pb is pa's parent or previous
            pa->sibling = pb->child;
            pb->child   = pa;
            pa->parent  = pb;
            return pb;
        }
        else {
            pb->sibling = pa->child;
            pa->child   = pb;
            pb->parent  = pa;
            return pa;
        }

        return nullptr; //impossible to reach?
    }

    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
    Node * root;
    size_t num_nodes;
};


#endif // PAIRINGPQ_H
