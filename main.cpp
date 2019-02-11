#include <iostream>
#include<cstdio>
#include <ctime>
#include <deque>
#include <vector>
#include <random>
#include <cstdlib>
#include <queue>
#include <thread>
#include <mutex>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <set>

using namespace std;

mutex mtx;
int population_number = 2, turn = 0, year = 0, living = 2;
class Cat;
vector<Cat> cats;
vector<string> male, female;
vector<char> used;
vector<Cat> alive;

class Plate{
    private: deque<int> food;
    public:
    Plate(){
        int n=5+(rand()&16);
        for(int i=0; i<n; i++){
            food.push_back(rand()%16-5);
        }
    }
    int feed(){
        if(!food.empty()){
            int unit=food.front();
            food.pop_front();
            return unit;
        }
        else return -100;
    }

    void addFood(){
        food.push_back(rand()%16-5);
    }

    void addFood(int n){
        for(int i=0; i<n; i++)
            food.push_back(rand()%16-5);
    }

    bool isEmpty(){
        return food.size()!=0;
    }
};

class Cat{
    private:
        string name;
        int hp, starvation, age, level, partner, born;
        int Max_HP, Max_Starv, power, exp, Agr_Starv, Need_exp;
        bool gender, isMarried, alive;
    public:
        vector<int> children;
        int ind, kidded;
        Cat(){
            kidded = 0;
            gender = rand()%2;
            hp = starvation=50;
            age = 0;
            alive = true;
            born = turn;
            int i = rand()%200;
            name = gender ? male[i] : female[i];
            isMarried = false;
            gender = bool(rand()%2);
            level = rand()%5;
            exp = 0; Max_HP = 100; Max_Starv = 100; Agr_Starv = 60, Need_exp = 5;
        }

        void setCharacteristics(int age, int starvation, int hp, string name, char gender, int level){
            this->age=age;
            this->starvation=starvation;
            this->gender=(gender=='m');
            this->name=name;
            this->hp=hp;
            this->level = level;
        }

        Plate eat(Plate plate){
            int n = plate.feed();
            if(n==-100) printf("Empty plate");
            else{
                starvation-=n;
            }
            return plate;
        }

        void marry(int pr){
            partner = pr;
            isMarried = true;
        }

        Cat makeChild(Cat pr){
            Cat kitten;
            kitten.level = min(level, pr.level);
            children.push_back(population_number);
            kitten.ind = population_number;
            return kitten;
        }

        void showCharacteristics(Cat pr){
            cout <<"Name: " << name << "\n";
            cout << "Gender: " << (gender ? "male\n" : "female\n");
            printf("Age: %d\nHP: %d\nHunger: %d\n", age, hp, starvation);
            if(isMarried) cout << "Partner: " << pr.name << "\n";
            printf("\n");
            return;
        }

        void showChildren(int depth){
            printf("%d.", depth);
            used[ind] = true;
            if(isMarried)
                used[partner] = true;
            for(int i=depth; i>0; i--)
                printf("  ");
            printf("Lvl.%d ", level);
            cout << name;
            if(!alive) printf(" dead");
            //printf(" st: %d", starvation);
            if(isMarried) cout <<" married with Lvl." << cats[partner].level << " " << cats[partner].name << (cats[partner].isAlive() ? "\n" : " dead\n");
            else cout << "\n";
            if(children.size()>0){
                for(unsigned int i=0; i<children.size(); i++)
                    if(!used[cats[children[i]].ind])
                        cats[children[i]].showChildren(depth+1);
            }
        }

        int getLevel(){
            return level;
        }

        bool getGender(){
            return gender;
        }

        string getName(){
            return name;
        }

        bool getIsMarried(){
            return isMarried;
        }

        int getPartner(){
            return partner;
        }

        int getAge(){
            return age;
        }

        int getStarvation(){
            return starvation;
        }

        void die(){
            this->alive = false;
            living--;
        }

        int maxHP(){
            return Max_HP;
        }

        int maxStarvation(){
            return Max_Starv;
        }

        void levelUp(){
            exp-=Need_exp;
            level++;
            Need_exp+=5*level;
            Max_HP += 10;
            Max_Starv += 10;
        }

        void updateStatus(){
            age = (turn-born)/5;
            if(starvation>(Max_Starv-20-5*level)) {
                hp-=1+(starvation-(Max_Starv-20-5*level))/2;
                exp++;
            }
            starvation+=1;
            if(hp<1) die();
            if(hp<Max_HP&&starvation<(Max_Starv-30-5*level)) hp++;
            while(exp>Need_exp) levelUp();
        }

        bool isAlive(){
            return alive;
        }

};

bool operator <(Cat a, Cat b){
    return a.getLevel()<b.getLevel();
}

void dfs(int depth, int c){
    used[c] = true;
    for(int i=0; i<depth; i++) printf("  ");
    cout << cats[c].getName();
    if(cats[c].getIsMarried()&&cats[cats[c].getPartner()].isAlive()){
        cout << " married with " << cats[cats[c].getPartner()].getName() << "\n";
        used[cats[c].getPartner()]=true;
    }
    else cout << "\n";
    for(int i=0; i<cats[c].children.size(); i++)
        if(cats[cats[c].children[i]].isAlive()&&!used[cats[c].children[i]]) dfs(depth+1, cats[cats[c].children[i]].ind);
}

void world(){
    srand((unsigned)time(0));
    set<Cat> fem, mal;
    Cat a;
    Plate plate;
    deque<Cat> forFeed;
    a.setCharacteristics(3, 0, 100, "Adam", 'm', 0);
    cats.push_back(a);
    a.setCharacteristics(3, 0, 100, "Eva", 'f', 0);
    cats.push_back(a);
    cats[0].ind = 0;
    cats[1].ind = 1;
    int s = 2;
    while(true){
        mtx.lock();
        for(int i=0; i<population_number; i++){
            if(cats[i].isAlive()){
                if(cats[i].getGender()&&!cats[i].getIsMarried()&&cats[i].getAge()>2&&cats[i].getAge()<8) mal.insert(cats[i]);
                if(!cats[i].getGender()&&!cats[i].getIsMarried()&&cats[i].getAge()>2&&cats[i].getAge()<8) fem.insert(cats[i]);
                if(cats[i].getIsMarried()&&cats[i].isAlive()&&cats[cats[i].getPartner()].isAlive()){ //making children
                    if(turn-cats[i].kidded>2){
                        a = cats[i].makeChild(cats[cats[i].getPartner()]);
                        cats[cats[i].getPartner()].children.push_back(a.ind);
                        population_number++;
                        living++;
                        cats.push_back(a);
                        cats[i].kidded = turn;
                        cats[cats[i].getPartner()].kidded = turn;
                    }
            }
                if(cats[i].getStarvation()>40) forFeed.push_back(cats[i]);
                cats[i].updateStatus();
                if(cats[i].isAlive()) alive.push_back(cats[i]);
            }
        }
        while(!mal.empty()&&!fem.empty()){
            a = *(fem.begin());
            Cat b = *(mal.begin());
            //cout << a.getName() << " " << a.getIsMarried() << "\n" << b.getName() << " " << b.getIsMarried() << "\n";
            cats[a.ind].marry(b.ind);
            cats[b.ind].marry(a.ind);
            mal.erase(b);
            fem.erase(a);
        }

        used.resize(alive.size());
        for(int i=0; i<used.size(); i++) used[i] = false;

        while(!forFeed.empty()&&!plate.isEmpty()){
            int ind = forFeed.front().ind;
            if(!cats[ind].isAlive()){
                forFeed.pop_front();
                continue;
            }
            plate = cats[ind].eat(plate);
            if(cats[ind].getStarvation()<20) forFeed.pop_front();
        }

        system("cls");
        printf("Turn: %d\nYear: %d\n", turn, turn/5);
        printf("Total: %d\nAlive: %d\n", population_number, living);
        turn++;
        if(living==0) turn--;
        plate.addFood(living*4);
        Sleep(30);
        mtx.unlock();
    }

}

void pushThread(){
    while(true){
        mtx.lock();
        char c;
        if(kbhit()){
            c = getch();
            if(c=='p'){
                used.resize(cats.size());
                for(int i=0; i<used.size(); i++) used[i] = false;
                cats[0].showChildren(0);
                alive.clear();
                while(!kbhit());
                c = getch();
            }
        }

        mtx.unlock();
    }
}

int main()
{
    ifstream ml("maleNames.txt");
    ifstream fml("femaleNames.txt");
    string te;
    for(int i=0; i<200; i++){
        ml>>te;
        male.push_back(te);
        fml >> te;
        female.push_back(te);
    }
    thread sim(world);
    thread button(pushThread);

    sim.join();
    button.join();
}
