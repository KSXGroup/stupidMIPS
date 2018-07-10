#include<cstdio>
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<string>
#include<thread>
#include<map>
#include<mutex>
#include<condition_variable>
#include<vector>
#include<queue>
#include<functional>
#include<fstream>
using namespace std;

const int command_len = 10;
const int address_name_len = 50;
const int buffer_size = 20;

vector<char> _memory;
map<string, int> _label;

ifstream _file;

int hs_p;

template<class T>
void static_append(T &a)
{
    int n(sizeof(T));
    char *p(reinterpret_cast<char*> (&a));
    for(int i = 0; i < n; ++i, ++p)
        _memory.push_back(*p);
}

template<class T>
void str_append(T a, char *w)
{
    T *p(reinterpret_cast<T*>(w));
    *p = a;
}

int fetch_int(const char *b)
{
    const int *p(reinterpret_cast<const int*>(b));
    return *p;
}

bool _str_cmp(const char *a, const char *b)
{
    for(;*a == *b && (*a) != '\0'; ++a, ++b);
    return (*a) == '\0' && (*b) == '\0';
}

struct _command
{
    char com[command_len], add[3][address_name_len];
    _command()
    {
		memset(com, 0, sizeof(com));
        for(int i = 0; i < 3; ++i)
            memset(add[i], 0, sizeof(add[i]));
    }
};

struct _command_short
{
    char com[command_len], add[3][5], num;
    _command_short(): num(0)
    {
		memset(com, 0, sizeof(com));
        for(int i = 0; i < 3; ++i)
            memset(add[i], 0, sizeof(add[i]));
    }
};

bool is_number(char c)
{
    return c >= '0' && c <= '9';
}

void load_p()
{
    string s, s2;
    int status = -1;
    while(_file >> s)
    {
        if(s[s.length() - 1] == ':')
        {
            s.pop_back();
            _label[s] = _memory.size();
        }
        else if(s == ".data")
            status = 0;
        else if(s == ".text")
            status = 1;
        else if(s == ".align")
        {
            int n, t;
            _file >> n;
            n = (1 << n);
            t = (n - _memory.size() % n) % n;
            for(int i = 0; i < t; ++i)
                _memory.push_back(0);
        }
        else if(status == 0)
        {
            if (s[1] == 'a')
            {
                int mcnt(0);
                unsigned int i;
                do
                {
                    i = 0;
                    _file >> s2;
                    if (s2[0] == '\"')
                    {
                        i = 1;
                        ++mcnt;
                        if (mcnt == 2)
                            _memory.push_back(' ');
                    }
                    else
                        _memory.push_back(' ');
                    for (; i < s2.length() && s2[i] != '\"'; ++i)
                    {
                        if (s2[i] == '\\')
                        {
                            ++i;
                            switch (s2[i])
                            {
                            case 'a':
                                s2[i] = 7;
                                break;
                            case 'b':
                                s2[i] = 8;
                                break;
                            case 'f':
                                s2[i] = 12;
                                break;
                            case 'n':
                                s2[i] = 10;
                                break;
                            case 'r':
                                s2[i] = 13;
                                break;
                            case 't':
                                s2[i] = 9;
                                break;
                            case 'v':
                                s2[i] = 11;
                                break;
                            case '\\':
                                s2[i] = 92;
                                break;
                            case '\'':
                                s2[i] = 39;
                                break;
                            case '\"':
                                s2[i] = 34;
                                break;
                            case '\?':
                                s2[i] = 63;
                                break;
                            case '\0':
                                s2[i] = 0;
                                break;
                            }
                        }
                        _memory.push_back(s2[i]);
                    }
                } while (s2[i] != '\"' && mcnt != 2);
                if (s[s.length() - 1] == 'z')
                    _memory.push_back('\0');
            }
            else if (s == ".space")
            {
                int n;
                _file >> n;
                for (int i = 0; i < n; ++i)
                    _memory.push_back(0);
            }
            else
            {
                int n(1);
                if (s == ".half")
                    n = 2;
                if (s == ".word")
                    n = 4;
                int t;
                do
                {
                    _file >> t;
                    char *p(reinterpret_cast<char *>(&t));
                    for (int i = 0; i < n; ++i, ++p)
                        _memory.push_back(*p);
                } while (_file.get() == ',');
            }
        }
        else if (status == 1)
        {
            _command tmp;
            strcpy(tmp.com, s.c_str());
            if (s != "nop" && s != "syscall")
            {
                int i = 0;
                for (_file >> s; s[s.length() - 1] == ','; ++i, _file >> s)
                {
                    s.pop_back();
                    strcpy(tmp.add[i], s.c_str());
                }
                strcpy(tmp.add[i], s.c_str());
            }
            static_append(tmp);
        }
    }
}

class _register
{
  private:
    char v[4];
    int flag;
    mutex mtx;
    condition_variable_any cv;

  public:
    _register() : flag(0)
    {
        memset(v, 0, sizeof(v));
    }
    _register &operator=(const int &o)
    {
        mtx.lock();
        int *p(reinterpret_cast<int *>(v));
        *p = o;
		--flag;
        mtx.unlock();
        cv.notify_one();
        return *this;
    }
    _register &operator=(const char *s)
    {
        mtx.lock();
        strcpy(v, s);
		--flag;
        mtx.unlock();
        cv.notify_one();
        return *this;
    }
    _register &operator=(string s)
    {
        mtx.lock();
        strcpy(v, s.c_str());
		--flag;
        mtx.unlock();
        cv.notify_one();
        return *this;
    }
    void lock()
    {
        mtx.lock();
		++flag;
        mtx.unlock();
    }
    int get_int_at_risk()
    {
        mtx.lock();
        const int *p(reinterpret_cast<int *>(v));
        int tmp(*p);
        mtx.unlock();
        cv.notify_one();
        return tmp;
    }
    int get_int()
    {
        mtx.lock();
        while (flag)
            cv.wait(mtx);
        const int *p(reinterpret_cast<int *>(v));
        int tmp(*p);
        mtx.unlock();
        cv.notify_one();
        return tmp;
    }
    string get_str()
    {
        mtx.lock();
        while (flag)
            cv.wait(mtx);
        string r(v);
        mtx.unlock();
        cv.notify_one();
        return r;
    }
} _re[35];

class task_queue
{
  private:
    _command_short que[buffer_size];
    int st, en;
    condition_variable_any is_full, is_empty;
    mutex mtx;

  public:
    void run(void (*fun)(_command_short &))
    {
        int running = 1;
        while (running)
        {
            mtx.lock();
            while (st == en)
				is_empty.wait(mtx);
            fun(que[st]);
            if (que[st].com[0] == '@')
            {
                running = 0;
                continue;
            }
            ++st;
            if (st >= buffer_size)
                st -= buffer_size;
            mtx.unlock();
            is_full.notify_one();
        }
    }
    void add(_command_short &t)
    {
        mtx.lock();
        while ((en + 1) % buffer_size == st)
			is_full.wait(mtx);
        que[en] = t;
        ++en;
        if (en >= buffer_size)
            en -= buffer_size;
        mtx.unlock();
        is_empty.notify_one();
    }
    task_queue() : st(0), en(0){};
} L1, L2, L3, L4;

void init()
{
    hs_p = _memory.size();
    _memory.resize(hs_p + 4 * 1024 * 1024);
	_re[29].lock();
	_re[34].lock();
    _re[29] = _memory.size();
    if (_label.find("main") != _label.end())
        _re[34] = _label["main"];
    else
        _re[34] = -1;
}

void stage_zero()
{
    int cnt = 0;
    while (_re[34].get_int() > 0)
    {
        _command_short tmp;
        str_append(_re[34].get_int(), tmp.add[0]);
        char *t(&_memory[_re[34].get_int()]);
        _command *p(reinterpret_cast<_command *>(t));
		_re[34].lock();
        L1.add(tmp);
        if (!(p->com[0] == 'b' || p->com[0] == 'j' || (p->com[0] == 's' && p->com[1] == 'y')))
            _re[34] = _re[34].get_int_at_risk() + sizeof(*p);
    }
    _command_short tmp;
    tmp.com[0] = '@';
    L1.add(tmp);
}

int get_reg_id(const char *s)
{
    if (is_number(s[1]))
    {
        int val(0);
        for (int i = 1; is_number(s[i]); ++i)
        {
            val *= 10;
            val += s[i] - '0';
        }
        return val;
    }
    if (s[1] == 'z')
        return 0;
    if (s[1] == 'a')
    {
        if (s[2] == 't')
            return 1;
        return 4 + s[2] - '0';
    }
    if (s[1] == 'v')
        return 2 + s[2] - '0';
    if (s[1] == 't')
    {
        if (s[2] < '8')
            return 8 + s[2] - '0';
        else
            return 24 + s[2] - '8';
    }
    if (s[1] == 's')
    {
        if (s[2] == 'p')
            return 29;
        return 16 + s[2] - '0';
    }
    if (s[1] == 'g')
        return 28;
    if (s[1] == 'f')
        return 30;
    if (s[1] == 'r')
        return 31;
    if (s[1] == 'h')
        return 32;
    if (s[1] == 'l')
        return 33;
    if (s[1] == 'p')
        return 34;
}

int get_command_type(const char *s)
{
    return s[0] == 'b' || s[0] == 'j' || (s[0] == 's' && (s[1] == 'b' || s[1] == 'h' || s[1] == 'w'));
}

void anay(char *s, char *t, char *t2 = NULL)
{
	if(s[4] == 1)
	{
		str_append(fetch_int(s), t);
		return;
	}
    int re_id;
    if (s[0] == '$')
    {
        re_id = get_reg_id(s);
        str_append(_re[re_id].get_int(), t);
        return;
    }
    if (is_number(s[0]) || s[0] == '-')
    {
        int offset = 0;
        int flag = 1, i = 0;
        if (s[0] == '-')
        {
            flag = -1;
            i = 1;
        }
        while (is_number(s[i]))
        {
            offset *= 10;
            offset += s[i] - '0';
            ++i;
        }
        offset *= flag;
        str_append(offset, t);
        if (s[i] != '(')
            return;
        re_id = get_reg_id(&(s[i + 1]));
        str_append(_re[re_id].get_int(), t2);
        return;
    }
	re_id = _label[s];
	str_append(re_id, t);
	str_append(re_id, s);
	s[4] = 1;
}

void stage_one(_command_short &ob)
{
	static int cnt = 0;
    if (ob.com[0] == '@')
    {
        L2.add(ob);
        return;
    }
    int ad(fetch_int(ob.add[0]));
    int lock_id(-1);
    _command *p(reinterpret_cast<_command *>(&_memory[ad]));
    _command_short tmp;
    strcpy(tmp.com, p->com);
    if (fetch_int(p->add[0]) != 0)
    {
        int re_id;
        if (get_command_type(tmp.com) || (fetch_int(p->add[2]) == 0 && (tmp.com[0] == 'd' || (tmp.com[0] == 'm' && tmp.com[1] == 'u'))))
        {
            if (p->add[0][0] != '$')
            {
                if(p -> add[0][49] == 0)
                {
                    re_id = _label[p->add[0]];
                    str_append(re_id, p->add[0] + 1);
                    p -> add[0][49] = 1;
                }
                else
                    re_id = fetch_int(p -> add[0] + 1);
            }
            else
                re_id = _re[get_reg_id(p->add[0])].get_int();
            if (fetch_int(p->add[2]) == 0 && (tmp.com[0] == 'd' || (tmp.com[0] == 'm' && tmp.com[1] == 'u')))
            {
                _re[get_reg_id("$lo")].lock();
                _re[get_reg_id("$hi")].lock();
            }
            if (_str_cmp(tmp.com, "jal") || _str_cmp(tmp.com, "jalr"))
            {
                str_append(_re[34].get_int_at_risk(), tmp.add[1]);
                lock_id = 31;
            }
        }
        else
        {
            re_id = get_reg_id(p->add[0]);
            lock_id = re_id;
        }
        str_append(re_id, tmp.add[0]);
    }
    if (fetch_int(p->add[1]) != 0)
        anay(p->add[1], tmp.add[1], tmp.add[2]);
    if (fetch_int(p->add[2]) != 0)
    {
        anay(p->add[2], tmp.add[2]);
        tmp.num = 1;
    }
    if (tmp.com[0] == 'm' && tmp.com[1] == 'f')
        if (tmp.com[2] == 'h')
            str_append(_re[32].get_int(), tmp.add[1]);
        else
            str_append(_re[33].get_int(), tmp.add[1]);
    if (tmp.com[0] == 's' && tmp.com[1] == 'y')
    {
        int type(_re[get_reg_id("$v0")].get_int());
        str_append(type, tmp.add[0]);
        switch (type)
        {
        case 1:
        case 4:
            str_append(_re[get_reg_id("$a0")].get_int(), tmp.add[1]);
            break;
        case 5:
            lock_id = get_reg_id("$v0");
            break;
        case 8:
            str_append(_re[get_reg_id("$a0")].get_int(), tmp.add[1]);
            str_append(_re[get_reg_id("$a1")].get_int(), tmp.add[2]);
            break;
        case 9:
            str_append(_re[get_reg_id("$a0")].get_int(), tmp.add[1]);
            lock_id = get_reg_id("$v0");
            break;
        case 17:
            str_append(_re[get_reg_id("$a0")].get_int(), tmp.add[1]);
            break;
        }
    }
    if (lock_id >= 0)
	{
        _re[lock_id].lock();
	}
    L2.add(tmp);
}

void stage_two(_command_short &ob)
{
    if (ob.com[0] == 'a' && ob.com[1] == 'd')
        str_append(fetch_int(ob.add[1]) + fetch_int(ob.add[2]), ob.add[1]);
    if (ob.com[0] == 's' && ob.com[1] == 'u')
        str_append(fetch_int(ob.add[1]) - fetch_int(ob.add[2]), ob.add[1]);
    if (ob.com[0] == 'm' && ob.com[1] == 'u')
        if (ob.com[3] == 'u')
            if (ob.num)
            {
                unsigned int a(fetch_int(ob.add[1])), b(fetch_int(ob.add[2]));
                str_append(a * b, ob.add[1]);
            }
            else
            {
                unsigned long long a(fetch_int(ob.add[0])), b(fetch_int(ob.add[1]));
                str_append(a * b, ob.add[0]);
            }
        else if (ob.num)
        {
            int a(fetch_int(ob.add[1])), b(fetch_int(ob.add[2]));
            str_append(a * b, ob.add[1]);
        }
        else
        {
            long long a(fetch_int(ob.add[0])), b(fetch_int(ob.add[1]));
            str_append(a * b, ob.add[0]);
        }
    if (ob.com[0] == 'd' && ob.com[1] == 'i')
        if (ob.com[3] == 'u')
            if (ob.num)
            {
                unsigned int a(fetch_int(ob.add[1])), b(fetch_int(ob.add[2]));
                str_append(a / b, ob.add[1]);
            }
            else
            {
                unsigned int a(fetch_int(ob.add[0])), b(fetch_int(ob.add[1]));
                str_append(a / b, ob.add[0]);
                str_append(a % b, ob.add[1]);
            }
        else if (ob.num)
        {
            int a(fetch_int(ob.add[1])), b(fetch_int(ob.add[2]));
            str_append(a / b, ob.add[1]);
        }
        else
        {
            unsigned int a(fetch_int(ob.add[0])), b(fetch_int(ob.add[1]));
            str_append(a / b, ob.add[0]);
            str_append(a % b, ob.add[1]);
        }
    if (ob.com[0] == 'x' && ob.com[1] == 'o')
        str_append((fetch_int(ob.add[1]) ^ fetch_int(ob.add[2])), ob.add[1]);
    if (ob.com[0] == 'n' && ob.com[1] == 'e')
        if (ob.com[3] == 'u')
        {
            unsigned int a(fetch_int(ob.add[1]));
            str_append((~a), ob.add[1]);
        }
        else
            str_append(-fetch_int(ob.add[1]), ob.add[1]);
    if (ob.com[0] == 'r' && ob.com[1] == 'e')
        if (ob.com[3] == 'u')
        {
            unsigned int a(fetch_int(ob.add[1])), b(fetch_int(ob.add[2]));
            str_append(a % b, ob.add[1]);
        }
        else
            str_append(fetch_int(ob.add[1]) % fetch_int(ob.add[2]), ob.add[1]);
    if (_str_cmp(ob.com, "seq"))
        str_append((int)(fetch_int(ob.add[1]) == fetch_int(ob.add[2])), ob.add[1]);
    if (_str_cmp(ob.com, "sge"))
        str_append((int)(fetch_int(ob.add[1]) >= fetch_int(ob.add[2])), ob.add[1]);
    if (_str_cmp(ob.com, "sgt"))
        str_append((int)(fetch_int(ob.add[1]) > fetch_int(ob.add[2])), ob.add[1]);
    if (_str_cmp(ob.com, "sle"))
        str_append((int)(fetch_int(ob.add[1]) <= fetch_int(ob.add[2])), ob.add[1]);
    if (_str_cmp(ob.com, "slt"))
        str_append((int)(fetch_int(ob.add[1]) < fetch_int(ob.add[2])), ob.add[1]);
    if (_str_cmp(ob.com, "sne"))
        str_append((int)(fetch_int(ob.add[1]) != fetch_int(ob.add[2])), ob.add[1]);
    if (_str_cmp(ob.com, "beq"))
        str_append((int)(fetch_int(ob.add[0]) == fetch_int(ob.add[1])), ob.add[0]);
    if (_str_cmp(ob.com, "bne"))
        str_append((int)(fetch_int(ob.add[0]) != fetch_int(ob.add[1])), ob.add[0]);
    if (_str_cmp(ob.com, "bge"))
        str_append((int)(fetch_int(ob.add[0]) >= fetch_int(ob.add[1])), ob.add[0]);
    if (_str_cmp(ob.com, "ble"))
        str_append((int)(fetch_int(ob.add[0]) <= fetch_int(ob.add[1])), ob.add[0]);
    if (_str_cmp(ob.com, "bgt"))
        str_append((int)(fetch_int(ob.add[0]) > fetch_int(ob.add[1])), ob.add[0]);
    if (_str_cmp(ob.com, "blt"))
        str_append((int)(fetch_int(ob.add[0]) < fetch_int(ob.add[1])), ob.add[0]);
    if (_str_cmp(ob.com, "beqz"))
        str_append((int)(fetch_int(ob.add[0]) == 0), ob.add[0]);
    if (_str_cmp(ob.com, "bnez"))
        str_append((int)(fetch_int(ob.add[0]) != 0), ob.add[0]);
    if (_str_cmp(ob.com, "blez"))
        str_append((int)(fetch_int(ob.add[0]) <= 0), ob.add[0]);
    if (_str_cmp(ob.com, "bgez"))
        str_append((int)(fetch_int(ob.add[0]) >= 0), ob.add[0]);
    if (_str_cmp(ob.com, "bgtz"))
        str_append((int)(fetch_int(ob.add[0]) > 0), ob.add[0]);
    if (_str_cmp(ob.com, "bltz"))
        str_append((int)(fetch_int(ob.add[0]) < 0), ob.add[0]);
    if (_str_cmp(ob.com, "jal") || _str_cmp(ob.com, "jalr"))
        str_append(fetch_int(ob.add[1]) + sizeof(_command), ob.add[1]);
    if (_str_cmp(ob.com, "la") ||
        _str_cmp(ob.com, "lb") ||
        _str_cmp(ob.com, "lh") ||
        _str_cmp(ob.com, "lw") ||
        _str_cmp(ob.com, "sb") ||
        _str_cmp(ob.com, "sh") ||
        _str_cmp(ob.com, "sw"))
        str_append(fetch_int(ob.add[1]) + fetch_int(ob.add[2]), ob.add[1]);

    if (_str_cmp(ob.com, "beq") ||
        _str_cmp(ob.com, "bne") ||
        _str_cmp(ob.com, "bge") ||
        _str_cmp(ob.com, "ble") ||
        _str_cmp(ob.com, "bgt") ||
        _str_cmp(ob.com, "blt"))
    {
        if (fetch_int(ob.add[0]))
            _re[34] = fetch_int(ob.add[2]);
        else
            _re[34] = _re[34].get_int_at_risk() + sizeof(_command);
    }

    if (_str_cmp(ob.com, "beqz") ||
        _str_cmp(ob.com, "bnez") ||
        _str_cmp(ob.com, "bgez") ||
        _str_cmp(ob.com, "blez") ||
        _str_cmp(ob.com, "bgtz") ||
        _str_cmp(ob.com, "bltz"))
    {
        if (fetch_int(ob.add[0]))
            _re[34] = fetch_int(ob.add[1]);
        else
            _re[34] = _re[34].get_int_at_risk() + sizeof(_command);
    }

    if (_str_cmp(ob.com, "b"))
    {
        _re[34] = fetch_int(ob.add[0]);
    }

    if (_str_cmp(ob.com, "j") || _str_cmp(ob.com, "jr"))
    {
        _re[34] = fetch_int(ob.add[0]);
    }
    if (_str_cmp(ob.com, "jal") || _str_cmp(ob.com, "jalr"))
    {
        _re[31] = _re[34].get_int_at_risk() + sizeof(_command);
        _re[34] = fetch_int(ob.add[0]);
    }

    if (_str_cmp(ob.com, "syscall"))
    {
        int type(fetch_int(ob.add[0]));
        switch (type)
        {
        case 10:
        case 17:
            _re[34] = -1;
            break;
        default:
            _re[34] = _re[34].get_int_at_risk() + sizeof(_command);
        }
    }
    L3.add(ob);
}

void stage_three(_command_short &ob)
{
    if (ob.com[0] == 'l' && ob.com[1] != 'a' && ob.com[1] != 'i')
    {
        int n = 1;
        if (ob.com[1] == 'h')
            n = 2;
        if (ob.com[1] == 'w')
            n = 4;
        int ad(fetch_int(ob.add[1]));
        str_append(0, ob.add[1]);
        for (int i = 0; i < n; ++i, ++ad)
            ob.add[1][i] = _memory[ad];
    }
    if (ob.com[0] == 's' && (ob.com[1] == 'b' || ob.com[1] == 'h' || ob.com[1] == 'w'))
    {
        int n = 1;
        if (ob.com[1] == 'h')
            n = 2;
        if (ob.com[1] == 'w')
            n = 4;
        int ad(fetch_int(ob.add[1]));
        for (int i = 0; i < n; ++i, ++ad)
            _memory[ad] = ob.add[0][i];
    }
    if (_str_cmp(ob.com, "syscall"))
    {
        int type(fetch_int(ob.add[0]));
        switch (type)
        {
        case 1:
        {
            printf("%d", fetch_int(ob.add[1]));
            break;
        }
        case 4:
        {
            int ad(fetch_int(ob.add[1]));
            while (_memory[ad] != '\0')
                printf("%c", _memory[ad++]);
            break;
        }
        case 5:
        {
            int tmp;
            cin >> tmp;
            str_append(tmp, ob.add[1]);
            break;
        }
        case 8:
        {
            int ad(fetch_int(ob.add[1])), len(fetch_int(ob.add[2]));
            --len;
            string tmp;
            cin >> tmp;
            int l(tmp.length());
            l = min(l, len);
            for (int i = 0; i < l; ++i, ++ad)
                _memory[ad] = tmp[i];
            _memory[ad] = '\0';
            break;
        }
        case 9:
        {
            int len(fetch_int(ob.add[1]));
            str_append(hs_p, ob.add[1]);
            hs_p += len;
            break;
        }
        }
    }
    L4.add(ob);
}

void stage_four(_command_short &ob)
{
    if ((ob.com[0] == 'a' && ob.com[1] == 'd') ||
        (ob.com[0] == 's' && ob.com[1] == 'u') ||
        (ob.com[0] == 'x' && ob.com[1] == 'o') ||
        (ob.com[0] == 'n' && ob.com[1] == 'e') ||
        (ob.com[0] == 'r' && ob.com[1] == 'e') ||
        (_str_cmp(ob.com, "seq")) ||
        (_str_cmp(ob.com, "sle")) ||
        (_str_cmp(ob.com, "slt")) ||
        (_str_cmp(ob.com, "sge")) ||
        (_str_cmp(ob.com, "sgt")) ||
        (_str_cmp(ob.com, "sne")))
        _re[fetch_int(ob.add[0])] = fetch_int(ob.add[1]);

    if ((ob.com[0] == 'm' && ob.com[1] == 'u') || (ob.com[0] == 'd' && ob.com[1] == 'i'))
        if (ob.num)
            _re[fetch_int(ob.add[0])] = fetch_int(ob.add[1]);
        else
        {
            _re[get_reg_id("$lo")] = fetch_int(ob.add[0]);
            _re[get_reg_id("$hi")] = fetch_int(ob.add[1]);
        }

    if (ob.com[0] == 'l')
        _re[fetch_int(ob.add[0])] = fetch_int(ob.add[1]);
    if (_str_cmp(ob.com, "move") || _str_cmp(ob.com, "mfhi") || _str_cmp(ob.com, "mflo"))
        _re[fetch_int(ob.add[0])] = fetch_int(ob.add[1]);


    if (_str_cmp(ob.com, "syscall"))
    {
        int type(fetch_int(ob.add[0]));
        switch (type)
        {
        case 5:
            _re[get_reg_id("$v0")] = fetch_int(ob.add[1]);
            break;
        case 9:
            _re[get_reg_id("$v0")] = fetch_int(ob.add[1]);
            break;
        }
    }
}

int main(int args, char *argv[])
{
    std::ios::sync_with_stdio(false);
    _file.open(argv[1]);
    load_p();
    _file.close();
    init();
    thread task1(bind(&task_queue::run, &L1, placeholders::_1), stage_one);
    thread task2(bind(&task_queue::run, &L2, placeholders::_1), stage_two);
    thread task3(bind(&task_queue::run, &L3, placeholders::_1), stage_three);
    thread task4(bind(&task_queue::run, &L4, placeholders::_1), stage_four);
    stage_zero();
    task1.join();
    task2.join();
    task3.join();
    task4.join();
    return 0;
}

