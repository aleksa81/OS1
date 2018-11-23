#ifndef TLIST_H_
#define TLIST_H_

#include <conio.h>
#include <dos.h>
#include <iostream.h>

template<class T>
class TList{
public:
	class Node{
	public:
		Node *next;
		Node *prev;
		T* data;
		Node(T* data){
			this->data = data;
			prev=0;
			next=0;
		}
		~Node(){
			//cprintf("~Node()\r\n");
		}
	};
private:
	Node *head;
	Node *tail;
	int counter;

public:
	TList(){head=tail=0; counter=0;}
	~TList(){
		Node *temp = head;
		while (head!=0){
			head=head->next;
			//delete temp->data; brisu se rucno
			delete temp;
			temp=head;
		}
		head=tail=0;
	}

	void append(Node *newNode){
		if (head == 0){
				head = tail = newNode;
		}
		else{
			tail->next = newNode;
			newNode->prev = tail;
			tail = tail->next;
		}
		counter++;
	}

	T* getFirst(){
		if (head != 0){
			Node *temp = head;
			head=head->next;
			T* ret = temp->data;
			delete temp;
			if (head != 0) head->prev = 0;
			else tail = 0;
			return ret;
		}
		else return 0;
	}

	int isEmpty()const{return head==0;}

	inline void printList()const{
		for (Node* temp = head; temp != 0; temp=temp->next){
			//cout << temp->data->getId() << " ";
		}
		cout << endl;
	}

	Node* remove(Node* target){ // brise i data
		if (target==0 || head==0) return 0;

		TList<T>::Node* temp = 0;

		if (target->prev==0) head = head->next;
		else target->prev->next = target->next;

		if (target->next==0) tail = tail->prev;
		else target->next->prev = target->prev;

		temp = target->next;

		delete target->data;
		delete target;
		counter--;
		return temp;
	}

	void removeT(T* target){ //brise samo node
		if (target==0 || head==0) return;
		TList<T>::Node *temp = head;
		while (temp!=0){
			if (temp->data==target) break;
			else temp=temp->next;
		}
		if (temp==0) return;

		if (temp->prev==0) head = head->next;
		else temp->prev->next = temp->next;

		if (temp->next==0) tail = tail->prev;
		else temp->next->prev = temp->prev;

		delete temp;
		counter--;
	}

	Node* removeNode(Node* target){ //brise samo node
			TList<T>::Node* temp = 0;

			if (target->prev==0) head = head->next;
			else target->prev->next = target->next;

			if (target->next==0) tail = tail->prev;
			else target->next->prev = target->prev;

			temp = target->next;

			delete target;
			counter--;
			return temp;
		}

protected:
	friend class Thread;
	friend class Daemon;
	friend class PCB;
	friend class KernelSem;
	friend class KernelSemList;
};

#endif /* TLIST_H_ */
