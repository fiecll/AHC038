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
vector<char> DIR = {'R', 'D', 'L', 'U'}; //移動方向を表す文字
vector<char> direction  = {'R', 'L','.'}; //回転操作を表す文字

struct Point{
    int id;  // 頂点id
    Point* parent; //親頂点
    vector<Point*> children; //子供頂点
    pair<int, int> position; //頂点の位置 
    bool have; //たこ焼きをつかんでいるかどうか

    // 子供用のコンストラクタ
    Point(int id, Point* parent, pair<int,int> position, bool have = false)
        : id(id), parent(parent), position(position), have(have){
            children = {};
        }

    // 根用のコンストラクタ
    Point(int id, pair<int, int> position) 
        : id(id), parent(nullptr), position(position), have(false) {
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
        point->position.first = parent->position.first  + dy;   // x' = y
        point->position.second = parent->position.second - dx; // y' = -x
    }
    else if (direction == 1) { // 反時計回り
        point->position.first = parent->position.first  - dy;   // x' = -y
        point->position.second = parent->position.second + dx; // y' = x
    }
    }
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
    // for(int i=0;i<n;i++){
    //     for(int j = 0;j<n;j++){
    //         if(takoyaki_position[i][j])cout << 1;
    //         else cout << 0;

    //     }
    //     cout << endl;
    // }
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
    //    for(int i=0;i<n;i++){
    //     for(int j = 0;j<n;j++){
    //         cout << target[i][j] ;
    //     }
    //     cout << endl;
    // }

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

    cout << v << endl;
    // アームの初期化
    Point* root = new Point(0, make_pair(0,0));
    arm.push_back(root);
    // 0を親として、頂点idの長さのアームを設定
    //v = 2;
    for(int i=1;i<v;i++){
        cout << 0 << ' ' << i << endl;
        Point* newpoint = new Point(i, arm[0], make_pair(arm[0]->position.first , arm[i-1]->position.second+1));
        arm.push_back(newpoint);
        arm[0]->children.push_back(newpoint);  // 子供を登録
    }

    //とりあえず根の初期位置は原点に設定
    cout << 0 << ' ' << 0 << endl;
    auto start = chrono::high_resolution_clock::now();
    vector<string>realans;
    int lastscore = 1e9;
    const int MAX_ITERATIONS = 10000;
    while(true){
        int score = 0;
        vector<string>ans;//暫定答え
        // // アームの状態を初期化
        // for(auto &a : arm){
        //     if(a->id != 0){
        //         a->position = make_pair(arm[0]->position.first + a->id, arm[0]->position.second);
        //         a->have = false;
        //     }
        // }
        int turn = 0;

        while(hasRemainingTargets()){
            // if(score == 50)break;
            // cout << "turn:" << score << ' ';
            // cout << arm[0]->position.first <<' ' <<  arm[0]->position.second;
            // cout << arm[1]->position.first <<' ' <<  arm[1]->position.second<<endl;
            if(score >= MAX_ITERATIONS){
                break;
            }
            string s;

            // 移動操作の指定
            int dir = rand() % 4;
            int new_x = arm[0]->position.first + DX[dir];
            int new_y = arm[0]->position.second + DY[dir];
            if(new_x >= 0 && new_x < n && new_y >= 0 && new_y < n){
                move(arm[0], dir);
                s += DIR[dir];
            } else {
                s += '.';
            }

            // cout << arm[0]->position.first << ' ' << arm[0]->position.second ;
            // cout << arm[1]->position.first << ' ' << arm[1]->position.second << endl;
            // 回転操作の指定
            for(int i = 1; i < v; i++){
                if (arm[i]->children.empty()) {
                    int rot = rand() % 3;
                    rotate(arm[i], rot);
                    s += direction[rot];
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
                            s += 'P';
                        }
                        else if(arm[i]->have == true && target[x][y] == true && takoyaki_position[x][y] == false){
                            arm[i]->have = false;
                            target[x][y] = false; // たこ焼きを置く
                            takoyaki_position[x][y] = 1;
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

            ans.push_back(s);
            score ++;
        }

        if(score < lastscore){
            lastscore = score;
            realans = ans;
        }

        // auto now = chrono::high_resolution_clock::now();

        // // 経過時間を計算
        // chrono::duration<double> elapsed = now - start;
        // if(elapsed.count()>= 2.98){
        //     break;
        // }
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
