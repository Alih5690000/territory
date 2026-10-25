#include <vector>
#include <iostream>
#include <algorithm>
#include <windows.h>

struct Team{
    char color;
    int id;
};

const Team TEAM_DEFAULT=Team{'.', 0};

struct Pixel {
    int x,y;
    Team team;
    std::vector<Pixel*>& t;
    float attack;
    float defense;
    Pixel(int x_, int y_, Team t, float att, float def,
        std::vector<Pixel*>& tt):
        x(x_),y(y_),team(t),attack(att),defense(def),t(tt){}
    bool Invade(Pixel* p){
        if (attack>0 && p->team.id!=team.id){
            p->team=team;
            attack-=p->defense;
            p->attack=0;
            p->t.erase(
                std::remove(p->t.begin(), 
                p->t.end(), p), p->t.end());
            return true;
        }
        return false;
    }
};

typedef std::vector<std::vector<Pixel>> Map;

std::vector<Pixel*> GetNeighbors(Map& m, const Pixel& p){
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    std::vector<Pixel*> neighbors;
    for (int i = 0; i < 4; ++i) {
        int nx = p.x + dx[i];
        int ny = p.y + dy[i];
        if (ny >= 0 && ny < m.size() &&
                nx >= 0 && nx < m[ny].size()) {
            neighbors.push_back(&m[ny][nx]);
        }
    }
    return neighbors;
}

class Territory{
    public:
    Team team;
    Map& map;
    float AttackBoost=1.f;
    float DefenseBoost=1.f;
    float MaxAttack=10;
    float MaxDefense=1;
    float attack=10;
    float defense=1.f;
    float oldDefense=defense;
    std::vector<Pixel*> ps;
    std::vector<Pixel*> getBorders(){
        std::vector<Pixel*> borders;
        for (const auto& p : ps) {
            if (p->team.id != team.id)
                continue;
            auto neighbors = GetNeighbors(map, *p);
            for (const auto& n : neighbors) {
                if (n->team.id != team.id) {
                    borders.push_back(p);
                    break;
                }
            }
        }
        return borders;
    }
    void invade(Territory ttt){
        int team=ttt.team.id;
        int invaded=0;
        auto bs=getBorders();
        float left=0.f;
        ttt.InitDefense();
        for (auto i:bs){
            auto ns=GetNeighbors(map, *i);
            if (team==-1){
                std::vector<Pixel*> available;
                for (auto j : ns) {
                    if (j->team.id != this->team.id)
                        available.push_back(j);
                }
                if (!available.empty()) {
                    i->attack=attack*AttackBoost+left;
                    float budget1=attack;
                    bool sucess=false;
                    float sum=0;
                    int val=0;
                    std::vector<Pixel*> visited;
                    while(budget1>0){
                        if (visited.size()==available.size()
                            || i->attack<=0.f) break;
                        Pixel* j;
                        while(true){
                            j = available[rand() % available.size()];
                            if (std::find(visited.begin(), 
                                visited.end(), j) == visited.end()){
                                    visited.push_back(j);
                                    break;
                            }   
                        }
                        if(i->Invade(j) && 
                            std::find(ps.begin(), ps.end(), j) == ps.end()){
                            ps.push_back(j);
                            invaded++;
                            sucess=true;
                            budget1-=j->defense;
                            sum+=j->defense;
                            val++;
                        }
                        else{
                            if (j->team.id!=this->team.id)
                                CancelDefense(j);
                        }
                    }
                    left=std::max(0.f, i->attack);
                    if (sucess) attack-=sum/val;
                }
            }
            else{
                for (auto j:ns){
                    float sum=0;
                    int val=0;
                    if (j->team.id==team){
                        i->attack=attack*AttackBoost+left;
                        bool sucess=false;
                        if(i->Invade(j) && 
                            std::find(ps.begin(), ps.end(), j) == ps.end()){
                            ps.push_back(j);
                            invaded++;
                            sucess=true;
                            sum+=j->defense;
                            val++;
                        }
                        else{
                            if (j->team.id!=this->team.id)
                                CancelDefense(j);
                        }
                        if (sucess) {
                            attack-=sum/val;
                            left=std::max(0.f, i->attack);
                        }
                    }
                }
            }
        }
        if (left>0.f) invade(ttt);
    }
    float part;
    void InitDefense(){
        auto bs=getBorders();
        if (bs.empty()) return;
        part=defense/bs.size();
        for (auto i:bs){
            i->defense+=defense*DefenseBoost;
        }
        oldDefense=defense;
        defense=0;
    }
    void CancelDefense(Pixel* p){
        p->defense-=part;
        defense+=part;
    }
    void rest(float am){
        defense=std::min(defense+am, MaxDefense);
        attack=std::min(attack+am, MaxAttack);
    }
    Territory(Map& ma, std::vector<Pixel*> p={}):map(ma){
        ps=p;
    }
};

std::vector<Pixel*> EMPTY={};
Map EMPTY_MAP;

Map CreateMap(int w, int h){
    Map res;
    for (int i=0;i<h;i++){
        std::vector<Pixel> a;
        for (int j=0;j<w;j++){
            a.push_back(Pixel{j,i,TEAM_DEFAULT, 10, 10, 
                EMPTY});
        }
        res.push_back(a);
    }
    return res;
}

void PrintMap(const Map& m){
    for (auto i:m){
        for (auto j:i){
            std::cout<<j.team.color<<' ';
        }
        std::cout<<std::endl;
    }
}

const auto EMPTY_LANDS=[](Map& m){
    auto t=Territory(m);
    t.team.id=-1;
    return t;
};

void reset_console_cursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(hConsole, pos);
}

int main(){
    srand(time(NULL));
    Map map=CreateMap(20, 20);
    Territory t(map, {&map[0][0]});
    map[0][0].team={'#', 1};
    t.attack=10;
    t.defense=10;
    t.team={'#', 1};
    Territory tt(map, {&map[0][19]});
    map[0][19].team={'@', 2};
    tt.attack=40;
    tt.defense=20;
    tt.team={'@', 2};
    for (;;){
        t.invade(EMPTY_LANDS(map));
        tt.invade(EMPTY_LANDS(map));
        t.rest(5);
        tt.rest(20);
        PrintMap(map);
        reset_console_cursor();
    }
    /*t.rest(5);
    tt.rest(5);*/
    PrintMap(map);
}
