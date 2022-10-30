#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <vector>

using namespace std;

#define Nw 40000  //number of random walkers
#define L 400    //grid size
#define Ns 5000  //number of steps (time intervals, delta t) in the simulation
#define randomize_frequency 0.01    //probability that a walker will move to a random point on the grid in the current step

#define deathrate 0.005       //deathrate of walkers with no immunity
#define infection_probability 0.2   //probability of infection if coming into contact with no immunity
#define disease_time 20       //in every step an infected walker has a 1/disease_time chance of recovering or dying

#define inf_decay0 0.999     //infection immunity decay factor over 1 time interval
#define mask_protection_factor 0.5     //a person with a mask has a 100*mask_protection_factor % lower chance of getting infected
#define mask_frequency 0.8     //chance that a random random walker is wearing a mask


//random number generators***********************************************************
float ran1(){   //generates a random float between 0 and 1
    return (float)rand() / (RAND_MAX); 
}

short ranL(){   //generates a random integer between 0 and L(not included)
    return rand()%L;
}

int ranN(int N){    //generates a random integer between 0 and N(not included)
    return rand()%N;
}

short ran9(){   //generates a random integer between 0 and 8(included)
    return rand()%9;
}

short ran100(){ //generates a random number between 0 and 99(included)
    return rand()%100;
}
//************************************************************************************


void infect_area(bool a[L][L],int x,int y, bool hasmask);     //marks all cells surrounding infected walkers
float gr_rate(int a[14], int i);        //calculates the ratio between new cases in the last 7 time intervals and new cases in the previous 7 time intervals




//class of random walkers
class walker{
private:
    bool isinfected;    //infection status
    bool hasmask;   //determines whether the walker has a mask or not
    short position[2];      //(x,y) position on the grid 
    float inf_imm_decay;   //infection decay factor

public:
    float inf_immunity, d_immunity;     //infection and death immunity

    //constructor
    walker(){isinfected=0;inf_immunity=0.;inf_imm_decay=inf_decay0;inf_immunity=0.;d_immunity=0.; hasmask=0;}

    //coordinate functions
    short get_x(){return position[0];}
    short get_y(){return position[1];}
    void random_position(){position[0]=ranL();position[1]=ranL();}
    void walk(){
        immdecay();     //immunity decays over time
        switch (ran9())     //change to ran1() and if-else if statements if you introduce mobility restrictions
        {
        case 0: position[0]-=1; position[1]+=1; break; //move up-left
        case 1: position[1]+=1; break; //move up
        case 2: position[0]+=1; position[1]+=1; break; //up-right
        case 3: position[0]-=1; break; //left
        case 4: position[0]+=1; break; //right
        case 5: position[0]-=1; position[1]-=1; break; //down-left
        case 6: position[1]-=1; break; //down
        case 7: position[0]+=1; position[1]-=1; break; //down-right
        default: break; //don't move
        }
        //fixing the coordinates in case they went off the grid, it's the same in void infect_area(bool a[L][L],int x,int y, bool hasmask)
        position[0] = (position[0]+L)%L;
        position[1] = (position[1]+L)%L;
    }


    //infection functions
    bool infected(){return isinfected;}     //returns infected status
    void infect(){isinfected=1;}       //marks walker as infected
    bool infection_update(){        //updates the status of the infected walker
        float r = ran1();
        if(r<1./disease_time){    //walker stops being infected
            if(r<deathrate*(1.-d_immunity)/disease_time)   return 0;   //walker dies
            else{   //walker recovers
                isinfected=0;
                if(inf_immunity>0 && inf_imm_decay)     //walker already had immunity so his new immunity decays slower
                {
                    inf_imm_decay = (1.+inf_decay0)/2;
                }
                inf_immunity=1.;
                d_immunity=0.5;     //factor by which the deathrate is multiplied after the first infection
                return 1;
            }
        }
        return 1;   //walker remains infected
    }
    bool contact(){     //updates the status of the walker who had contact with the infection
        if(ran1()<infection_probability*inf_susceptibility()) return isinfected=1;
        return 0;
    }

    //protection/immunity functions
    float inf_susceptibility(){     //returns level of susceptibility to the disease 
        return (1-inf_immunity)*(1-mask_protection());
    }
    void immdecay(){    //immunity decay
        inf_immunity*=inf_imm_decay;    //infection immunity
    }
    void give_mask(){hasmask=1;}    //gives the walker a mask
    bool mask_status(){return hasmask;}     //returns the walkers mask status
    float mask_protection(){    //how protected is the walker by using masks (0 if he doesn't have a mask)
        if(hasmask){
            return mask_protection_factor;
        }
        return 0;
    }
};




//main part of the code**********************************************************************************************************************************************************************************
int main(){

    srand (time(NULL)); //random number seed (using current time)

    int N=Nw; //number of walkers who are alive
    int Nd=0, dNd; //total number of deaths, deaths in current step
    int TNi=0, Ni=0, dNi; //total number of infected walkers from t=0, current number of infected walkers, new infections
    int Nrec=0, dNrec; //number of recovered walkers, new recoveries

    int gr[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};   //growth rate array for storing the new cases number over the last 14 time steps

    walker *arr = new walker[Nw];           //memory allocation in case the array becomes too large
    vector<walker> w(arr, arr + Nw);    //using a dynamic array is useful here because walkers who die can't participate in the epidemic simulation anymore

    bool infection_area[L][L];      //area of infection, if infection_area[x][y]==1, a healthy walker standing at (x,y) can get infected


    //creating file pointers
    fstream fpositions, fNwalkers, ftotcases, factcases, fnewcases, fdeaths, fnewdeaths, fgrowthrate;

    //opening files
    fpositions.open("walker_positions.dat", ios::out);  //stores the random walker status as well (infected, recovered etc.), used for animations
    fNwalkers.open("Nwalkers.dat", ios::out);

    ftotcases.open("total_cases.dat", ios::out);
    factcases.open("active_cases.dat", ios::out);
    fnewcases.open("new_cases.dat", ios::out);

    fdeaths.open("deaths.dat", ios::out);
    fnewdeaths.open("new_deaths.dat", ios::out);

    fgrowthrate.open("gr.dat", ios::out);


    fpositions << "#x,  y, status\n";
    fNwalkers << "#t \t number of living walkers\n";
    ftotcases << "#t \t total number of cases\n";
    factcases << "#t \t number of active cases\n";
    fnewcases << "#t \t number of new cases\n";
    fdeaths << "#t \t total number of deaths\n";
    fnewdeaths << "#t \t number of new deaths\n";
    fgrowthrate << "#t \t growth rate(new cases in the last 7 steps divided by the number of new cases in the previous 7 steps)\n";

    w[0].infect(); Ni++; TNi++;  //first case

    //setting up random walkers on the LxL grid and giving them masks
    for(int i=0; i<Nw;i++){
        w[i].random_position();

        if(ran1()<mask_frequency) w[i].give_mask();     //every new measure/attribute needs to be randomly distributed to avoid having all vaccinated walkers having masks for example (unless you want that)

        fpositions << w[i].get_x() << "\t" << w[i].get_y() <<  "\t" << w[i].infected() << "\t" << w[i].inf_susceptibility() << "\t" << w[i].mask_status() << "\n";
    }


    //the code commented below prints out the infection area for t=0
    // cout << "x=" << w[0].get_x() << "\t" << "y=" << w[0].get_y() << endl;

    // for(int i=0; i<L; i++){     //infection area reset
    // for(int j=0; j<L; j++){
    //     infection_area[i][j]=0;
    // }
    // }

    // for(int i=0; i<N; i++){     //infection area in this step
    //     if(w[i].infected()){
    //         infect_area(infection_area, w[i].get_x(), w[i].get_y(), w[i].mask_status());
    //     }
    // }

    // for(int i=0; i<L;i++){
    //         for(int j=0; j<L;j++) cout << infection_area[j][L-i] << " ";
    //         cout << endl;
    //     }

    int is; //current step counter
    for(is=0; is<Ns; is++){     //time loop
        fpositions << "\n\n";   //new block

        fNwalkers << is << "\t" << N << "\n";
        ftotcases << is << "\t" << TNi << "\n";
        factcases << is << "\t" << Ni << "\n";
        fnewcases << is << "\t" << dNi << "\n";
        fdeaths << is << "\t" << Nd << "\n";
        fnewdeaths << is << "\t" << dNd << "\n";

        gr[is%14] = dNi;    //growth rate array

        if(is>=14)   fgrowthrate << is << "\t" << gr_rate(gr, is) << "\n";   //we need 14 intervals to calculate the growth rate

        for(int i=0; i<L; i++){     //infection area reset
            for(int j=0; j<L; j++){
                infection_area[i][j]=0;
            }
        }

        for(int i=0; i<N; i++){     //infection area in this step
            if(w[i].infected()){
                infect_area(infection_area, w[i].get_x(), w[i].get_y(), w[i].mask_status());
            }
        }

        dNd=dNi=dNrec=0;    //resetting the "new" variables
        for(int i=0; i<N; i++){     //new walker status (remain healthy, get infected, remain infected or die)
            if(w[i].infected()){    //infected case
                if(w[i].infection_update()){    //true if walker is still alive
                    if(w[i].infected()==0)  //walker has recovered
                    {
                        Ni--; dNrec++; Nrec++;
                    }
                }
                else   //walker has died and is removed from the population
                {
                    w.erase(w.begin()+i);
                    N--; Ni--; dNd++; Nd++;
                    i--; continue;  //the next walker now has the same index i
                }
            }
            else{   //not infected case
                if(infection_area[w[i].get_x()][w[i].get_y()]){    //there is an infected neighbour
                    if(w[i].contact()){dNi++; Ni++;TNi++;} //contact resulted in infection
                }
            }
            fpositions << w[i].get_x() << "\t" << w[i].get_y() <<  "\t" << w[i].infected() << "\t" << w[i].inf_susceptibility()  << "\t" << w[i].mask_status()  << "\n";
        }

        if(Ni==0) {is++;break;}    //there are no more infected walkers, the epidemic is over

        for(int i=0; i<N; i++)  w[i].walk();    //random walk

        for(int i=0; i<randomize_frequency*N; i++) w[ranN(N)].random_position(); //turning on travel to random grid position
    }
    //print with some basic information about the simulation
    //cout << is << " " << Ni << " " << TNi;

    fNwalkers << is << "\t" << N << "\n";
    ftotcases << is << "\t" << TNi << "\n";
    factcases << is << "\t" << Ni << "\n";
    fnewcases << is << "\t" << dNi << "\n";
    fdeaths << is << "\t" << Nd << "\n";
    fnewdeaths << is << "\t" << dNd << "\n";

    fpositions.close();
    fNwalkers.close();
    ftotcases.close();
    factcases.close();
    fnewcases.close();
    fdeaths.close();
    fnewdeaths.close();
    fgrowthrate.close();
    return 0;
}
//**************************************************************************************************************************************************************************************************************




void infect_area(bool a[L][L],int x,int y,bool hasmask){     //marks all cells surrounding infected walkers
    int i2, j2;
    for(int i=x-1; i<=x+1;i++){
        for(int j=y-1; j<=y+1;j++){
            // i2 = i%L;   //works crossing the right grid boundary
            // j2 = j%L;   //works for crossing the top grid boundary
            
            // if(i==-1) i2 = L+i; //crossing the bottom grid boundary
            // if(j==-1) j2 = L+j; //crossing the left grid boundary

            //a[i2][j2]=1;

            i2 = (L+i)%L;
            j2= (L+j)%L;
            a[i2][j2] = 1;    //works when crossing any grid boundary

            //adding masks
            if(hasmask && (j-y)*(i-x)!=0)   a[i2][j2]=0;    //limiting the infection area to 5 nearest cells
        }
    }
}

float gr_rate(int a[14], int i){        //calculates the ratio between new cases in the last 7 time intervals and new cases in the previous 7 time intervals
    int s1=0, s2=0;
    for(int j=0; j<14; j++){
        if(j!=0 && j<=7) s1+=a[(i+j)%14];
        else s2+=a[(i+j)%14];
    }
    if(s1==0)   return 0.95;      //sometimes the number of new cases in 7 time intervals is 0 but the disease is still alive so we need to fix this potential NaN error
    return 1.0*s2/s1;
}