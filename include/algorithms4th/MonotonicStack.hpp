#pragma once
#include <stack>
#include <vector>
#include <unordered_map>

class MonotonicStack {
public:
    static std::vector<int> next_greater_element(const std::vector<int>& nums)
    {
        int n = nums.size();
        std::vector<int> ret(n, -1);
        std::stack<int> st;

        for (int i = 0; i < n; ++i) {
            while (!st.empty() && nums[i] > nums[st.top()]) {
                ret[st.top()] = nums[i];
                st.pop();
            }
            st.push(i);
        }
        return ret;
    }
    static std::vector<int> next_greater_element(const std::vector<int>& nums1, const std::vector<int>& nums2)
    {
        int n1 = nums1.size();
        int n2 = nums2.size();
        std::vector<int> ret(n1, -1);
        std::unordered_map<int, int> mp;
        std::stack<int> st;

        for (int i = 0; i < n2; ++i) {
            while (!st.empty() && nums2[i] > nums2[st.top()]) {
                mp[nums2[st.top()]] = nums2[i];
                st.pop();
            }
            st.push(i);
        }

        while (!st.empty()) {
            mp[nums2[st.top()]] = -1;
            st.pop();
        }

        for (int i = 0; i < n1; ++i) {
            ret[i] = mp[nums1[i]];
        }
        return ret;
    }

    static std::vector<int> next_greater_element_circular(const std::vector<int>& nums)
    {
        int n = nums.size();
        std::vector<int> ret(n, -1);
        std::stack<int> st;

        for (int i = 0; i < 2 * n; ++i) {
            int idx = i % n;
            while (!st.empty() && nums[idx] > nums[st.top()]) {
                ret[st.top()] = nums[idx];
                st.pop();
            }

            if (i < n) {
                st.push(i);
            }
        }
        return ret;
    }
private:
};