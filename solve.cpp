#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using namespace atcoder;
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define all(v) v.begin(), v.end()
using ll = long long;
using Graph = vector<vector<int>>;
using P = pair<int,int>;

vector<int> DX = {0, 1, 0, -1};//X座標の変化
vector<int> DY = {1, 0, -1, 0};//Y座標の変化
vector<char> DIR = {'R', 'D', 'L', 'U','.'}; //移動方向を表す文字
vector<char> direction  = {'R', 'L','.'}; //回転操作を表す文字
vector<int>muki;
struct Point{
    int id;  // 頂点id
    Point* parent; //親頂点
    vector<Point*> children; //子供頂点
    pair<int, int> position; //頂点の位置 
    bool have; //たこ焼きをつかんでいるかどうか
    int muki;

    // 子供用のコンストラクタ
    Point(int id, Point* parent, pair<int,int> position, bool have = false, int muki = 0)
        : id(id), parent(parent), position(position), have(have),muki(muki){
            children = {};
        }

    // 根用のコンストラクタ
    Point(int id, pair<int, int> position) 
        : id(id), parent(nullptr), position(position), have(false),muki(0) {
            children = {};
        }
};

int n;  // グリッドの大きさをグローバルに
vector<vector<bool>> target;          //目的地にたこ焼きが置かれているかどうか
vector<vector<bool>> takoyaki_position; // たこ焼きの初期位置　1:置かれている
vector<Point*> arm;                    // アームの頂点を格納する配列


// 移動関数：根がグリッド内で移動可能な場合、全ての子供を同方向に移動させる
void move(Point* point, int direction){
    assert(direction >= 0 && direction <= 3);
    
    // 根の移動先を計算
    int new_x = point->position.first  + DX[direction];
    int new_y = point->position.second + DY[direction];
    
    // 根がグリッド内に収まるか確認
    if(new_x >= 0 && new_x < n && new_y >= 0 && new_y < n){
        // 根を移動
        point->position.first  = new_x;
        point->position.second = new_y;
        int s = arm.size();
        // 子供を再帰的に移動（境界チェック不要）
        for (int i=1;i<s;i++) {
            Point* child  =arm[i];
            child->position.first  += DX[direction];
            child->position.second += DY[direction];
        }
    }
    // 根がグリッド外に出る場合は何もしない
}

// 回転させる関数
void rotate(Point* point, int direction) {
    // direction: 0 = 時計回り、1 = 反時計回り, 2 = 回転なし
    assert (direction == 0 || direction == 1 || direction == 2);
    if(direction == 2) return;
    // 親頂点の位置を取得
    Point* parent = point->parent;

    // 親が存在しない場合は回転を行わない
    if (parent == nullptr) {
        return;
    }
    int dx = point->position.first - parent->position.first;
    int dy = point->position.second - parent->position.second;
    int new_x, new_y;
    if (direction == 0) { // 時計回り
        point->position.first = parent->position.first  + dy;   // x' = y
        point->position.second = parent->position.second - dx; // y' = -x
    }
    else if (direction == 1) { // 反時計回り
        point->position.first = parent->position.first - dy;   // x' = -y
        point->position.second = parent->position.second + dx; // y' = x
    }
    // 子供の頂点を回転
    for (Point* child : point->children) {
        int dx = child->position.first  - parent->position.first;
        int dy = child->position.second - parent->position.second;
    if (direction == 0) { // 時計回り
        child->position.first = parent->position.first  + dy;   // x' = y
        child->position.second = parent->position.second - dx; // y' = -x
    }
    else if (direction == 1) { // 反時計回り
        child->position.first = parent->position.first  - dy;   // x' = -y
        child->position.second = parent->position.second + dx; // y' = x
    }
    }
}

int direct(pair<int,int>goal){
    if(arm[0]->position == goal) {
        return -1; // 目標地点に到達している場合
    }
    if(goal.first == -1 || goal.second == -1 ){
        return -1;
    }
    int ans = 4;
    int now_x = arm[0]->position.first;
    int now_y = arm[0]->position.second;
    int dif_x = goal.first - now_x;
    int dif_y = goal.second - now_y;
    if(abs(dif_x) > abs(dif_y)){
        if(now_x < goal.first) ans = 1; // 下方向
        else ans = 3; // 上方向
    }
    else {
        if(now_y < goal.second) ans = 0; // 右方向
        else ans = 2; // 左方向
    }
    // 移動可能か確認
    int new_x = now_x + DX[ans];
    int new_y = now_y + DY[ans];
    if(new_x >= 0 && new_x < n && new_y >= 0 && new_y < n){
        return ans;
    } else {
        return -1; // 移動不可
    }
}



//たこ焼きのより存在する方向を考える
int find_nearest_takoyaki(Point* point){
    int now_x = point->position.first;
    int now_y = point->position.second;
    int ans = -1;
    int dist = 2*n+1;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(takoyaki_position[i][j] == true && target[i][j] == false){
                int dif_x = now_x - i;
                int dif_y = now_y - j;
                int kyori = abs(dif_x) + abs(dif_y);
                if(kyori < dist){
                    dist = kyori; // distを更新
                    if(abs(dif_x) > abs(dif_y)){
                        if(now_x < i) ans = 1; // 下に移動
                        else ans = 3;          // 上に移動
                    }
                    else {
                        if(now_y < j) ans = 0; // 右に移動
                        else ans = 2;          // 左に移動
                    }
                }
            }
        }
    }
    if(ans == -1) ans = 0; // たこ焼きが見つからない場合のデフォルト方向
    return ans;
}

// 最も近い目的地を探す
int find_nearest_target(Point* point){
    int now_x = point->position.first;
    int now_y = point->position.second;
    int ans  = rand()%4;
    int dist = 2*n+1;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(target[i][j] == true && takoyaki_position[i][j] == false){
                int dif_x = now_x - i;
                int dif_y = now_y - j;
                int kyori = abs (dif_x) + abs (dif_y);
                if( kyori < dist){
                    dist = kyori;
                    if(abs(dif_x) > abs(dif_y)){
                        if(now_x < i) ans =  1;
                        else ans =  3;
                    }
                else {
                    if(now_y < j) ans =  0;
                    else ans =  2;
                    }          
                }
            }
        }
    }
    return ans;
}

// takoyakisearch : true 次のたこ焼き　false 次の目的地
pair<int,int> search_nextgoal(Point* point ,bool takoyakisearch){
    int now_x = point->position.first;
    int now_y = point->position.second;
    int dist = 2*n+1;
    pair<int,int> ans = make_pair(-1,-1) ;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(target[i][j] == !takoyakisearch && takoyaki_position[i][j] == takoyakisearch){
            vector<pair<int,int>>goal;
            for(int dir=0;dir<4;dir++){
                int next_x = i + DX[dir] * point->id;
                int next_y = j + DY[dir] * point->id;
                goal.push_back(make_pair(next_x, next_y));
            }
            for(int k=0;k<4;k++){
                if(goal[k].first < 0 || goal[k].first >= n || goal[k].second < 0 || goal[k].second >= n ){
                    continue;
                }
            int path = abs(goal[k].first - arm[0]->position.first) + abs(goal[k].second - arm[0]->position.second);
            if(dist > path ) {
                ans = goal[k];
            dist = path;
            muki[point->id] = k;
            }
            }
            }
        }
    }
    return ans;
}



void solve(){
    srand(time(0));
    int m,v;
    cin >> n >> m >> v; // n :グリッドの大きさ　m:たこ焼きの個数　v: ロボットアームの頂点数
    takoyaki_position.assign(n, vector<bool>(n, false));               
    for(int i=0;i<n;i++){
        string s;
        cin >> s;
        for(int j = 0;j<n;j++){
            if(s[j] == '1'){
                takoyaki_position[i][j] = true;
            }
            else if (s[j] == '0'){
                takoyaki_position[i][j] = false;
            }
        }
    }
    target.assign(n,vector<bool>(n,false));

    // 目的地の情報を初期化　true:まだ置かれていない
    for(int i=0;i<n;i++){
        string t;
        cin >> t;
        for(int j=0;j<n;j++){
            if(t[j] == '1'){
                target[i][j] = true;
            }
        }
    }  

    // 未処理の目標位置があるかどうかを判定する関数
    auto hasRemainingTargets = [&]()-> bool {
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(target[i][j] == true && takoyaki_position[i][j] == false){
                    return true;
                }
            }
        }
        return false;
    };
    // 盤上にたこ焼きが残っている
    auto restTakoyaki = [&]() -> bool {
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(takoyaki_position[i][j] == true && target[i][j]==false){
                    return true;
                }
            }
        }
        return false;
    };

    auto rotate_point = [&](Point* point) -> int {
        int ans = -1;
        int now_x = point->position.first;
        int now_y = point->position.second;
        if(point->parent == nullptr){
            return ans;
        }
        if(now_x >= 0 && now_x < n && now_y >= 0 && now_y < n){
            return ans ;
        }
        Point* parent = point->parent;
        int dx = now_x - parent->position.first;
        int dy = now_y - parent->position.second;
        int next_x;
        int next_y;
        next_x = parent->position.first  + dy;
        next_y = parent->position.second - dx;
        if(next_x >= 0 && next_x < n && next_y >= 0 && next_y < n){
            if((target[next_x][next_y] == false && takoyaki_position[next_x][next_y] == true && point->have == false ) ||
            (target[next_x][next_y] == true && takoyaki_position[next_x][next_y] == false  && point->have == true ))
             {
            ans = 0;
            return ans;
            }
        }
        next_x = parent->position.first  - dy;
        next_y = parent->position.second + dx;
        if(next_x >= 0 && next_x < n && next_y >= 0 && next_y < n){
         if((target[next_x][next_y] == false && takoyaki_position[next_x][next_y] == true && point->have == false ) || 
            (target[next_x][next_y] == true && takoyaki_position[next_x][next_y] == false  && point->have == true )){
            if(target[next_x][next_y] == false && takoyaki_position[next_x][next_y] == true){
            ans = 1;
            return ans;
            }
        }
        }
        return ans ;
    };

    //v = 2;
    cout << v << endl;
    // アームの初期化
    Point* root = new Point(0, make_pair(0,0));
    arm.push_back(root);
    // 0を親として、頂点idの長さのアームを設定
    for(int i=1;i<v;i++){
        cout << 0 << ' ' << i << endl;
        Point* newpoint = new Point(i, arm[0], make_pair(arm[0]->position.first , arm[i-1]->position.second+1));
        arm.push_back(newpoint);
        arm[0]->children.push_back(newpoint);  // 子供を登録
    }

    //とりあえず根の初期位置は原点に設定
    cout << 0 << ' ' << 0 << endl;
    vector<string>realans;
    int lastscore = 1e9;
    const int MAX_ITERATIONS = 10000;
    int rest = m;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(takoyaki_position[i][j] && target[i][j]){
                rest--;
            }
        }
    }
    muki.resize(v,0);
    
    while(true){
        int score = 0;
        vector<string>ans;//暫定答え
        //vector<bool> next_point(v,false);// 次にたこ焼きを運ぶ先を管理する
        pair<int,int> next_goal;
        pair<int,bool> nowmove; // int :id bool : true たこ焼きをsagasiteisu

        auto decide_next_goal = [&]() -> pair<int,int> {
            int takoyakisearch_direction = 2*n+1;
            int takoyakideliver_direction = 2*n+1;
            int tid= -1 ;
            int did = -1;
            bool move = false;
            pair<int,int>next_takoyaki = make_pair(-1,-1);
            pair<int,int>nextgoal = make_pair(-1,-1);
            for(int i=1;i<v;i++){
                if(arm[i]->have == false && restTakoyaki()){
                    next_takoyaki = search_nextgoal(arm[i],true);
                    takoyakisearch_direction = abs(arm[0]->position.first - next_takoyaki.first) + abs(arm[0]->position.second - next_takoyaki.second);
                    tid = i;
                }
            }
            for(int i=1;i<v;i++){
                if(arm[i]->have == true){
                nextgoal = search_nextgoal(arm[i],false);
                if(takoyakideliver_direction > (arm[0]->position.first - nextgoal.first) + abs(arm[0]->position.second - nextgoal.second)){
                takoyakideliver_direction = abs(arm[0]->position.first - nextgoal.first) + abs(arm[0]->position.second - nextgoal.second);
                did = i;
                }
                }
            }
            if(takoyakisearch_direction <= takoyakideliver_direction) {
                move = true;
                nowmove = make_pair(tid,move);
                // if(score <= 100){
                // cout << tid << endl;
                // }
                return next_takoyaki;
            }
            else{
                move = false;
                nowmove = make_pair(did,move);
                // if(score <= 100){
                // cout << did << endl;
                // }
                return nextgoal;
            }
        };

        bool decision_dir = false;
        while(rest>0){
            // if(score <= 10000){
            //     cout << next_goal.first << ' '  << next_goal.second << endl;
            // }
            if (!hasRemainingTargets() && !restTakoyaki())break;
            if(score >= 10000){
                break;
            }
            
            string s;
            int dir;// 縦横移動の方向
            if(decision_dir == false){
                next_goal = decide_next_goal();
                decision_dir = true;
            }
            dir = direct(next_goal);
            if(dir == -1){
                // 移動できない場合、decision_dirをリセットして新しい目標を設定
                decision_dir = false;
                s += '.'; // 移動なし
            } else {
                assert(dir>= 0 && dir <= 3);
                int new_x = arm[0]->position.first  + DX[dir];
                int new_y = arm[0]->position.second + DY[dir];
                if(new_x >= 0 && new_x < n && new_y >= 0 && new_y < n){
                    move(arm[0], dir);
                    s += DIR[dir];
                } else {
                    s += '.';
                }
            }

            // 回転操作の指定
            for(int i = 1; i < v; i++){
                if (arm[i]->children.empty()) {
                    int rot ;
                    if(rotate_point(arm[i]) != -1){
                        rot = rotate_point(arm[i]);
                    }
                    if(abs(next_goal.first - arm[0]->position.first) + abs(next_goal.second-arm[0]->position.second) <= 2){
                        if(nowmove.first = i && arm[nowmove.first]->muki != muki[i]){
                            rot = 1;
                        }
                    }
                    else rot = rand() % 3;
                    rotate(arm[i], rot);
                    s += direction[rot];
                    if(rot == 0){
                        arm[i]->muki ++;
                        arm[i]->muki += 4;
                        arm[i]->muki %= 4;
                    }
                    if(rot == 1){
                        arm[i]->muki --;
                        arm[i]->muki += 4;
                        arm[i]->muki %= 4;
                    }
                } else {
                    s += '.'; // 操作なし
                }
            }

            // たこ焼き操作の指定
            for(int i = 0; i < v; i++){
                if(i == 0){
                    s += '.';
                } else {
                    int x = arm[i]->position.first;
                    int y = arm[i]->position.second;
                    if (x >= 0 && x < n && y >= 0 && y < n) {
                        if(takoyaki_position[x][y] == true && arm[i]->have == false && target[x][y] == false){
                            arm[i]->have = true;
                            takoyaki_position[x][y] = false; // たこ焼きを拾う
                            //next_point[i] = true;
                            if(nowmove.first == i){
                            decision_dir = false;
                            }
                            s += 'P';
                        }
                        else if(arm[i]->have == true && target[x][y] == true && takoyaki_position[x][y] == false){
                            // たこ焼きを置く
                            arm[i]->have = false;
                            takoyaki_position[x][y] = true;
                            //next_point[i] = false;
            
                            rest--;
                            if(nowmove.first == i){
                            decision_dir = false;
                            }
                            s += 'P';
                        }
                        else {
                            s += '.';
                        }
                    } else {
                        s += '.';
                    }
                }
            }
            // if(arm[0]->position == next_goal){
              decision_dir = false;
            // }
            ans.push_back(s);
            score ++;
        }

        if(score < lastscore){
            lastscore = score;
            realans = ans;
        }

        break;
    }

    for(auto s:realans) cout << s << endl;

    // メモリ解放
    for(auto &a : arm){
        delete a;
    }
}

int main(){
    solve();
    return 0;
}
