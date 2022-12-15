#include<iostream>
using namespace std;

template<typename T> class sparse_arr
{
    public:
        T** arr;
        int rows;
        int cols;
        int nz;
        int *pos;
        int *temp;
        sparse_arr(int r,int c)
        {
            rows=r;
            cols=c;
            int x=r*c*sizeof(T);
            arr=new T *[x];
            for(int j=0;j<x;j++)
                arr[j]=new T[3];
            temp=new int[c];
            pos=new int[c+1];
            pos[0]=1;
            for(int i=0;i<c;i++)
                temp[i]=0;
        }
        void build_sparse(T** mat,int r,int c)
        {
            int non=0,k=0;
            for(int i=0;i<r;i++)
            {
                for(int j=0;j<c;j++)
                {
                    if(mat[i][j]!=(T)0)
                    {
                        arr[k][0]=i;
                        arr[k][1]=j;
                        arr[k][2]=mat[i][j];
                        temp[j]++;
                        non++;
                        k++;
                    }
                }
            }
            this->nz=non;
        }
        sparse_arr<T> add(sparse_arr<T> B)
        {
            sparse_arr<T> res(B.rows,B.cols);
            int x=0,y=0,z=0;
            while(x<this->nz && y<B.nz)
            {
                if(this->arr[x][0]==B.arr[y][0])
                {
                    if(this->arr[x][1]==B.arr[y][1])
                    {
                        T p=this->arr[x][2]+B.arr[y][2];
                        if(p!=(T)0)
                        {
                            res.arr[z][0]=B.arr[y][0];
                            res.arr[z][1]=B.arr[y][1];
                            res.arr[z][2]=p;
                            res.temp[(int)B.arr[y][1]]++;
                            z++;
                        }
                        x++;
                        y++;
                    }
                    else if(this->arr[x][1]<B.arr[y][1])
                    {
                        res.arr[z][0]=this->arr[x][0];
                        res.arr[z][1]=this->arr[x][1];
                        res.arr[z][2]=this->arr[x][2];
                        res.temp[(int)this->arr[x][1]]++;
                        x++;
                        z++;
                    }
                    else
                    {
                        res.arr[z][0]=B.arr[y][0];
                        res.arr[z][1]=B.arr[y][1];
                        res.arr[z][2]=B.arr[y][2];
                        res.temp[(int)B.arr[y][1]]++;
                        y++;
                        z++;
                    }
                }
                else if(this->arr[x][0]<B.arr[y][0])
                {
                    res.arr[z][0]=this->arr[x][0];
                    res.arr[z][1]=this->arr[x][1];
                    res.arr[z][2]=this->arr[x][2];
                    res.temp[(int)this->arr[x][1]]++;
                    x++;
                    z++;
                }
                else
                {
                    res.arr[z][0]=B.arr[y][0];
                    res.arr[z][1]=B.arr[y][1];
                    res.arr[z][2]=B.arr[y][2];
                    res.temp[(int)B.arr[y][1]]++;
                    y++;
                    z++;
                }
            }
            while (x<this->nz)
            {
                res.arr[z][0]=this->arr[x][0];
                res.arr[z][1]=this->arr[x][1];
                res.arr[z][2]=this->arr[x][2];
                res.temp[(int)this->arr[x][1]]++;
                z++;
                x++;
            }
            while (y<B.nz)
            {
                res.arr[z][0]=B.arr[y][0];
                res.arr[z][1]=B.arr[y][1];
                res.arr[z][2]=B.arr[y][2];
                res.temp[(int)B.arr[y][1]]++;
                y++;
                z++;
            }
            res.nz=z;
            return res;
        }
        sparse_arr<T> transpose()
        {
            sparse_arr<T> res(this->cols,this->rows);
            res.nz=this->nz;
            int prev=0;
            pos[0]=0;
            for(int i=1;i<=this->cols;i++)
            {
                pos[i]=prev+temp[i-1];
                prev=pos[i];
            }
            for(int j=0;j<this->nz;j++)
            {
                int cur_col=this->arr[j][1];
                int dest=pos[cur_col];
                res.arr[dest][0]=this->arr[j][1];
                res.arr[dest][1]=this->arr[j][0];
                res.arr[dest][2]=this->arr[j][2];
                pos[cur_col]++;
            }
            return res;
        }
        sparse_arr<T> mul(sparse_arr<T> B)
        {
            sparse_arr res(this->rows,B.cols);
            sparse_arr bt(B.cols,B.rows);
            bt=B.transpose();
            int x=0,y=0,z=0,cur_r=0,cur_c=0,dest_r=0,dest_c=0;
            while(x<this->nz)
            {
                cur_r=this->arr[x][0];
                cur_c=this->arr[x][1];
                dest_r=cur_r;
                y=0;
                while(y<bt.nz)
                {
                    dest_c=bt.arr[y][0];
                    if(cur_c==bt.arr[y][1])
                    {
                        T val=this->arr[x][2]*bt.arr[y][2];
                        int p=0;
                        while(p<z)
                        {
                            if(res.arr[p][0]==dest_r && res.arr[p][1]==dest_c)
                            {
                                res.arr[p][2]+=val;
                                break;
                            }
                            p++;
                        }
                        if(p==z)
                        {
                            res.arr[z][0]=dest_r;
                            res.arr[z][1]=dest_c;
                            res.arr[z][2]=val;
                            z++;
                        }
                    }
                    y++;
                }
                x++;
            }
            res.nz=z;
            sparse_arr<T> C(res.rows,res.cols);
            int c_in=0;
            for(int i=0;i<z;i++)
            {
                if(res.arr[i][2]!=(T)0)
                {   
                    C.arr[c_in][0]=res.arr[i][0];
                    C.arr[c_in][1]=res.arr[i][1];
                    C.arr[c_in][2]=res.arr[i][2];
                    C.temp[(int)res.arr[i][1]]++;
                    int x=c_in;
                    while (x>0 && ((C.arr[x][0]==C.arr[x-1][0] && C.arr[x][1]<C.arr[x-1][1]) || C.arr[x][0]<C.arr[x-1][0]))
                    {
                        T temp1[3];
                        for(int p=0;p<3;p++)
                        {
                            temp1[p]=C.arr[x][p];
                            C.arr[x][p]=C.arr[x-1][p];
                            C.arr[x-1][p]=temp1[p];
                        }
                        x--;
                    }
                    c_in++;
                }
            }
            C.nz=c_in;
            return C;
        }
        void display()
        {
            if(this->nz==0)
            {
                cout<<"No elements to display"<<endl;
                return;
            }
            for(int i=0;i<this->nz;i++)
                cout<<this->arr[i][0]<<" "<<this->arr[i][1]<<" "<<this->arr[i][2]<<endl;
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
        sparse_arr<datatype> X(row1,col1);
        X.build_sparse(A,row1,col1);
        //cout<<"A"<<endl;
        //X.display();
        sparse_arr<datatype> Y(row2,col2);
        Y.build_sparse(B,row2,col2);
        //cout<<"B"<<endl;
        //Y.display();
        sparse_arr<datatype> Z(row1,col2);
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
        sparse_arr<datatype> X(row,col);
        X.build_sparse(A,row,col);
        //cout<<"A"<<endl;
        //X.display();
        sparse_arr<datatype> Z(col,row);
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
        sparse_arr<datatype> X(row1,col1);
        X.build_sparse(A,row1,col1);
        //cout<<"A"<<endl;
        //X.display();
        sparse_arr<datatype> Y(row2,col2);
        Y.build_sparse(B,row2,col2);
        //cout<<"B"<<endl;
        //Y.display();
        sparse_arr<datatype> Z(row1,col2);
        Z=X.mul(Y);
        cout<<"Prod"<<endl;
        Z.display();
    }
    return 0;
}