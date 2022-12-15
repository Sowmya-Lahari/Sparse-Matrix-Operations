#include<iostream>
using namespace std;
template<typename T> struct Node
{
    int row;
    int col;
    T val;
    Node* next;
};

template<typename T> class sparse_list
{
    public:
        Node <T>* head;
        Node <T>* tail;
        int rows;
        int cols;
        int nz;
        sparse_list(int r,int c)
        {
            head=NULL;
            tail=NULL;
            rows=r;
            cols=c;
        }
        void display()
        {
            if(this->nz==0)
            {
                cout<<"No elements to display\n";
                return;
            }
            Node<T>* temp;
            for(temp=this->head;temp!=NULL;temp=temp->next)
                cout<<temp->row<<" "<<temp->col<<" "<<temp->val<<endl;
        }
        void build_mat(T** mat,int r,int c)
        {
            int non=0;
            for(int i=0;i<r;i++)
            {
                for(int j=0;j<c;j++)
                {
                    T x=mat[i][j];
                    if(mat[i][j]!=0)
                    {
                        this->insert_end(i,j,x);
                        non++;
                    }
                }
            }
            this->nz=non;
        }
        void insert_end(int r, int c, T elem)
        {
            Node <T>* newnode=new Node<T>;
            newnode->row=r;
            newnode->col=c;
            newnode->val=elem;
            newnode->next=NULL;
            if(head==NULL)
            {
                head=newnode;
                tail=newnode;
            }
            else
            {
                tail->next=newnode;
                tail=tail->next;
            }
        }
        sparse_list<T> add(sparse_list<T> B)
        {
            sparse_list<T> res(B.rows,B.cols);
            Node<T>* temp1=this->head;
            Node<T>* temp2=B.head;
            while (temp1!=NULL && temp2!=NULL)
            {
                if(temp1->row==temp2->row) 
                {
                    if(temp1->col==temp2->col)
                    {
                        T p=temp1->val+temp2->val;
                        if(p!=0) 
                            res.insert_end(temp1->row,temp1->col,p);
                        temp1=temp1->next;
                        temp2=temp2->next;
                    }
                    else if(temp1->col<temp2->col)
                    {
                        res.insert_end(temp1->row,temp1->col,temp1->val);
                        temp1=temp1->next;
                    }
                    else
                    {
                        res.insert_end(temp2->row,temp2->col,temp2->val);
                        temp2=temp2->next;
                    }
                }
                else if(temp1->row<temp2->row)
                {
                    res.insert_end(temp1->row,temp1->col,temp1->val);
                    temp1=temp1->next;
                }
                else
                {
                    res.insert_end(temp2->row,temp2->col,temp2->val);
                    temp2=temp2->next;
                }
            }
            while (temp1!=NULL)
            {
                res.insert_end(temp1->row,temp1->col,temp1->val);
                temp1=temp1->next;
            }
            while (temp2!=NULL)
            {
                res.insert_end(temp2->row,temp2->col,temp2->val);
                temp2=temp2->next;
            }
            return res;
        }
        void insert_pos(int r,int c,T elem)
        {
            Node <T>* newnode=new Node<T>;
            newnode->row=r;
            newnode->col=c;
            newnode->val=elem;
            newnode->next=NULL;
            Node <T>* temp=head;
            if(this->head==NULL)
            {
                head=newnode;
                tail=newnode;
            }
            else if(r<head->row || (r==head->row && c<head->col))
            {
                newnode->next=head;
                head=head->next;
            }
            else if(r==head->row && c==head->col)
            {
                T x=head->val+elem;
                if(x==0)
                    head=head->next;
                else
                    head->val=x;
            }
            else
            {
                Node<T> *prev=head;
                while(temp!=NULL)
                {
                    if(temp->row==r && temp->col==c)
                    {
                        T x=temp->val+elem;
                        if(x==0)
                            temp=temp->next;
                        else
                            temp->val=x;
                        return;
                    }
                    if((temp->row>r) || (temp->row==r && temp->col>c))
                        break;
                    prev=temp;
                    temp=temp->next;
                }
                if(temp==NULL)
                {
                    prev->next=newnode;
                    tail=newnode;
                }
                else
                {
                    newnode->next=temp;
                    prev->next=newnode;
                }
            }
        }
        sparse_list<T> transpose()
        {
            sparse_list<T> res(this->cols,this->rows);
            Node<T>* temp=this->head;
            cout<<"Here"<<endl;
            while(temp!=NULL)
            {
                res.insert_pos(temp->col,temp->row,temp->val);
                temp=temp->next;
            }
            return res;
        }
        sparse_list<T> mul(sparse_list B)
        {
            sparse_list<T> res(this->rows,B.cols);
            Node<T>* temp1=this->head;
            Node<T>* temp2=B.head;
            while (temp1!=NULL)
            {
                temp2=B.head;
                while (temp2!=NULL)
                {
                   if(temp1->col==temp2->row)
                   {
                       T num=temp1->val*temp2->val;
                       if(num!=(T)0)
                       {
                           res.insert_pos(temp1->row,temp2->col,num);
                       }
                   } 
                   temp2=temp2->next;
                }
                temp1=temp1->next;
            }
            return res;
        }
};

int main()
{
    typedef float datatype;
    int op;
    cin>>op;
    if(op==1)//Addition
    {
        int row1,col1,row2,col2;
        cin>>row1>>col1;
        datatype **A;
        datatype **B;
        A=new datatype*[row1];
        for(int i=0;i<row1;i++)
        {
            A[i]=new datatype[col1];
            for(int j=0;j<col1;j++)
            {
                cin>>A[i][j];
            }
        }
        cin>>row2>>col2;
        B=new datatype*[row2];
        for(int i=0;i<row2;i++)
        {
            B[i]=new datatype[col2];
            for(int j=0;j<col2;j++)
            {
                cin>>B[i][j];
            }
        }
        sparse_list<datatype> X(row1,col1);
        X.build_mat(A,row1,col1);
        //cout<<"A"<<endl;
        //X.display();
        sparse_list<datatype> Y(row2,col2);
        Y.build_mat(B,row2,col2);
        //cout<<"B"<<endl;
        //Y.display();
        sparse_list<datatype> Z(row1,col2);
        Z=X.add(Y);
        cout<<"Sum"<<endl;
        Z.display();
    }
    else if(op==2)
    {
        int row,col;
        cin>>row>>col;
        datatype **A;
        A=new datatype*[row];
        for(int i=0;i<row;i++)
        {
            A[i]=new datatype[col];
            for(int j=0;j<col;j++)
            {
                cin>>A[i][j];
            }
        }
        sparse_list<datatype> X(row,col);
        X.build_mat(A,row,col);
        //cout<<"A"<<endl;
        //X.display();
        sparse_list<datatype> Z(col,row);
        Z=X.transpose();
        Z.display();
    }
    else if(op==3)
    {
        int row1,col1,row2,col2;
        cin>>row1>>col1;
        datatype **A;
        datatype **B;
        A=new datatype*[row1];
        for(int i=0;i<row1;i++)
        {
            A[i]=new datatype[col1];
            for(int j=0;j<col1;j++)
            {
                cin>>A[i][j];
            }
        }
        cin>>row2>>col2;
        B=new datatype*[row2];
        for(int i=0;i<row2;i++)
        {
            B[i]=new datatype[col2];
            for(int j=0;j<col2;j++)
            {
                cin>>B[i][j];
            }
        }
        sparse_list<datatype> X(row1,col1);
        X.build_mat(A,row1,col1);
        //cout<<"A"<<endl;
        //X.display();
        sparse_list<datatype> Y(row2,col2);
        Y.build_mat(B,row2,col2);
        //cout<<"B"<<endl;
        //Y.display();
        sparse_list<datatype> Z(row1,col2);
        Z=X.mul(Y);
        cout<<"Prod"<<endl;
        Z.display();
    }
    return 0;
}