#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include "bits-stdc++.h"
#include <string>

using namespace std;

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos)
        {
            pos = str.length();
        }
        string token = str.substr(prev, pos - prev);
        if (!token.empty())
        {
            tokens.push_back(token);
        }
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());

    return tokens;
}

template<typename T>
void column_drop(vector<int> drops, vector<vector<T>>& tdata)
{
    sort(drops.begin(), drops.end());
    for (int k = 0; k < drops.size(); k++)
    {
        if (k > 0)
        {
            drops[k] = drops[k] - 1;
        }
        for (int i = 0; i < tdata.size(); i++)
        {
            tdata[i].erase(tdata[i].begin() + drops[k]);
        }

    }
}

vector<vector<double>> readprepareTraindataset(const char* fname)
{
    vector<string> data;
    vector<vector<double>> tdata;
    ifstream file(fname);
    string line = "";
    int u = 0;
    while (getline(file, line))
    {
        if (line != "")
        {
            for (int i = 0; i + 1 < line.length(); i++)
            {
                if (line[i] == ',' && line[i + 1] == ',')
                {
                    line.insert(i + 1, " ");
                }
            }
            data.push_back(line);
            u++;
        }

    }

    file.close();

    tdata.resize(data.size() - 1);

    for (int i = 1; i < data.size(); i++)
    {
        vector<string> str = split(data[i], ",");

        for (int j = 0; j < str.size(); j++)
        {
            tdata[i - 1].push_back(atof(str[j].c_str()));

        }
    }

    for (int i = 0; i < tdata.size(); i++)
    {
        tdata[i].push_back(1);
    }

    vector<int> drops = { 0,1,2 };
    column_drop(drops, tdata);

    return tdata;

}

vector<pair<double, double>> get_new_centroid(vector<vector<double>>& tdata, const int k)
{
    vector<pair<double, double>> centroids;

    for (int x = 0; x < k; x++)
    {
        double sumX = 0, sumY = 0, N1 = 0, N2 = 0;
        for (int i = 0; i < tdata.size(); i++)
        {
            if (tdata[i][tdata[i].size() - 1] == x)
            {
                sumX += tdata[i][0];
                N1++;
            }
            if (tdata[i][tdata[i].size() - 1] == x)
            {
                sumY += tdata[i][1];
                N2++;
            }

        }
        sumX = sumX / N1;
        sumY = sumY / N2;
        centroids.push_back(make_pair(sumX, sumY));

    }

    return centroids;

}

vector<pair<double, double>> get_initial_centroid(vector<vector<double>>& tdata, const int k)
{
    map<double, double> sorted;

    for (int i = 0; i < tdata.size(); i++)
    {
        sorted.insert(make_pair(tdata[i][0], tdata[i][1]));
    }

    double each = ceil(sorted.size() / k);

    vector<pair<double, double>> centroids;
    double sumX = 0, sumY = 0;
    int p = 1;
    for (int i = 0; i < tdata.size(); i++)
    {
        sumX += tdata[i][0];
        sumY += tdata[i][1];

        if (i == p * (each - 1))
        {
            centroids.push_back(make_pair((sumX / each), (sumY / each)));
            sumX = 0;
            sumY = 0;
            p++;
        }
    }

    return centroids;
}

double cal_euclidean_dis(const double x1, const double x2, const double y1, const double y2)
{
    return sqrt((pow((x1 - x2), 2) + pow((y1 - y2), 2)));
}

vector<double> get_euclidean_matrix(vector<vector<double>>& tdata, vector<pair<double, double>>& centroid, const int k)
{
    vector<vector<double>> distances(tdata.size(), vector<double>(k));

    for (int z = 0; z < k; z++)
    {
        for (int i = 0; i < tdata.size(); i++)
        {
            double dis = cal_euclidean_dis(tdata[i][0], centroid[z].first, tdata[i][1], centroid[z].second);

            distances[i][z] = dis;
        }
    }


    vector<double> minassign(tdata.size());


    for (int i = 0; i < distances.size(); i++)
    {
        double minval = 10000000;
        int minindex = 0;
        for (int j = 0; j < distances[i].size(); j++)
        {
            if (minval > distances[i][j])
            {
                minval = distances[i][j];
                minindex = j;
            }

        }
        minassign[i] = minindex;


    }

    return minassign;
}

void assign_label(vector<vector<double>>& tdata, vector<double>& minassign)
{
    for (int i = 0; i < tdata.size(); i++)
    {
        tdata[i][tdata[0].size() - 1] = minassign[i];

    }
}


void start_clustering(vector<vector<double>>& tdata, const int k, const int iteration_limit)
{
    vector<pair<double, double>> initials = get_initial_centroid(tdata, k);
    vector<double> arr = get_euclidean_matrix(tdata, initials, k);
    assign_label(tdata, arr);

    int h = 0;
    while (h < iteration_limit)
    {
        vector<pair<double, double>> centroids = get_new_centroid(tdata, k);
        vector<double> arrmin = get_euclidean_matrix(tdata, centroids, k);
        assign_label(tdata, arrmin);
        h++;
    }

}

int main()
{
    vector<vector<double>> tdata = readprepareTraindataset("datasets_42674_74935_Mall_Customers.csv");

    start_clustering(tdata, 5, 30);

    for (int i = 0; i < tdata.size(); i++)
    {
        for (int j = 0; j < tdata[i].size(); j++)
        {
            cout << tdata[i][j] << " ";
        }
        cout << '\n';
    }

    return 0;
}
