#pragma once

/*
* 
*		MADE BY NBQ
* 
*/

#define NOMINMAX

#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <variant>
#include <unordered_map>
#include <optional>
#include <charconv>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#define LOG_CLR_NORMAL 0x7
#define LOG_CLR_RED FOREGROUND_RED
#define LOG_CLR_GREEN FOREGROUND_GREEN
#define LOG_CLR_BLUE FOREGROUND_BLUE
#define LOG_CLR_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN)

using value_t = std::variant<double, std::string>;

namespace n_math
{
	double relative_change(double x, double y)
	{
		if (!y) { return 0.0; }
		return ((x - y) / y);
	}

	std::vector<double> relative_change_vector(const std::vector<double>& x)
	{
		if (x.empty()) { return {}; }

		std::vector<double> relative_changes;
		relative_changes.reserve(x.size() - 1);
		for (size_t i = 1; i < x.size(); ++i) { relative_changes.emplace_back(n_math::relative_change(x[i], x[i - 1])); }

		return relative_changes;
	}

	double log_relative_change(double x, double y)
	{
		if (!y) { return 0.0; }
		double p = x / y;
		if (p <= 0.0) { return p; }
		return std::log(x / y);
	}

	std::vector<double> log_relative_change_vector(const std::vector<double>& x)
	{
		if (x.empty()) { return {}; }

		std::vector<double> relative_changes;
		relative_changes.reserve(x.size() - 1);
		for (size_t i = 1; i < x.size(); ++i) { relative_changes.emplace_back(n_math::log_relative_change(x[i], x[i - 1])); }

		return relative_changes;
	}

	std::vector<double> first_diff(const std::vector<double>& x)
	{
		if (x.empty()) { return {}; }

		std::vector<double> dx(x.size() - 1);
		for (size_t i = 1; i < x.size(); ++i) { dx[i - 1] = x[i] - x[i - 1]; }
		return dx;
	}
}

class c_logger
{
private:

	c_logger() {}

	~c_logger() {}

	c_logger(const c_logger&) = delete;

	c_logger& operator=(const c_logger&) = delete;

public:

	static c_logger& get() noexcept
	{
		static c_logger instance;
		return instance;
	}

	bool log(const std::string& msg, unsigned short color) noexcept
	{
		if (msg.empty()) { return false; }
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		std::cout << msg;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), LOG_CLR_NORMAL);
		return true;
	}

	bool log_formatted(const std::string& msg, unsigned short color, int width) noexcept
	{
		if (msg.empty()) { return false; }
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
		std::cout << std::setw(width) << std::left << msg;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), LOG_CLR_NORMAL);
		return true;
	}
};

class c_dataframe
{
private:

	std::unordered_map<std::string, std::vector<value_t>> m_data;
	size_t m_rows = 0;


	static bool _is_blank(std::string_view sv)
	{
		for (unsigned char c : sv)
		{
			if (!std::isspace(c)) { return false; }
		}
		return true;
	}

	static std::optional<double> _parse_double(std::string_view sv)
	{
		if (sv.empty()) { return std::nullopt; }

		auto eq = [&](std::string_view t) {
			if (sv.size() != t.size()) { return false; }
			for (size_t i = 0; i < sv.size(); ++i)
			{
				if ((char)std::tolower((unsigned char)sv[i]) != t[i]) { return false; }
			}
			return true;
			};

		if (eq("nan")) { return std::numeric_limits<double>::quiet_NaN(); }
		if (eq("inf") || eq("+inf")) { return std::numeric_limits<double>::infinity(); }
		if (eq("-inf")) { return -std::numeric_limits<double>::infinity(); }

		double v = 0.0;
		auto* b = sv.data(); auto* e = b + sv.size();
		auto [p, ec] = std::from_chars(b, e, v);
		if (ec == std::errc() && p == e) { return v; }

		return std::nullopt;
	}

	static std::string _value_to_string(const value_t& value)
	{
		return std::visit([](auto&& val) -> std::string {

			using t = std::decay_t<decltype(val)>;

			if constexpr (std::is_same_v<t, std::string>)
			{
				return val;
			}
			else
			{
				std::ostringstream oss;
				oss.imbue(std::locale::classic());
				oss << val;
				return oss.str();
			}
			}, value);
	}

	static value_t _to_value(const value_t& v)
	{
		return v;
	}

	static value_t _to_value(value_t&& v)
	{
		return std::move(v);
	}

	static value_t _to_value(double v)
	{
		return value_t{ v };
	}

	static value_t _to_value(size_t v)
	{
		return value_t{ static_cast<double>(v) };
	}

	static value_t _to_value(long long v)
	{
		return value_t{ static_cast<double>(v) };
	}

	static value_t _to_value(int v)
	{
		return value_t{ static_cast<double>(v) };
	}

	static value_t _to_value(bool v)
	{
		return value_t{ v ? 1.0 : 0.0 };
	}

	static value_t _to_value(const std::string& s)
	{
		return value_t{ s };
	}

	static value_t _to_value(std::string&& s)
	{
		return value_t{ std::move(s) };
	}

	static value_t _to_value(std::string_view sv)
	{
		return value_t{ std::string(sv) };
	}

	static value_t _to_value(const char* s)
	{
		return value_t{ std::string(s ? s : "") };
	}

	static value_t _to_value(char* s)
	{
		return value_t{ std::string(s ? s : "") };
	}

	template <std::size_t n>
	static value_t _to_value(const char(&lit)[n]);

	template <class t>
	static value_t _to_value(const t&) = delete;

	static std::vector<value_t> _to_column(const std::vector<value_t>& v)
	{
		return v;
	}

	static std::vector<value_t> _to_column(std::vector<value_t>&& v)
	{
		return std::move(v);
	}

	template<class t>
	static std::vector<value_t> _to_column(const std::vector<t>& v);

	template<class t>
	static std::vector<value_t> _to_column(std::vector<t>&& v);

	static std::vector<std::string> _parse_csv_line(const std::string& line)
	{
		std::vector<std::string> fields;

		std::string cur;
		cur.reserve(line.size());

		bool in_quotes = false;
		for (size_t i = 0; i < line.size(); ++i)
		{
			char c = line[i];
			if (in_quotes)
			{
				if (c == '"') {
					if (i + 1 < line.size() && line[i + 1] == '"') { cur.push_back('"'); ++i; }
					else { in_quotes = false; }
				}
				else cur.push_back(c);
			}
			else
			{
				if (c == ',') { fields.emplace_back(std::move(cur)); cur.clear(); }
				else if (c == '"') { in_quotes = true; }
				else { cur.push_back(c); }
			}
		}

		fields.emplace_back(std::move(cur));

		return fields;
	}

	bool _append_column(const std::string& name, std::vector<value_t> values)
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return false;
		}

		if (values.empty())
		{
			c_logger::get().log("[c_dataframe]: No values\n", LOG_CLR_RED);
			return false;
		}

		if (this->m_rows == 0) { this->m_rows = values.size(); }
		else if (values.size() != this->m_rows)
		{
			c_logger::get().log("[c_dataframe]: Row length mismatch: " + name + "\n", LOG_CLR_RED);
			return false;
		}

		auto [it, inserted] = this->m_data.emplace(name, std::move(values));
		if (!inserted)
		{
			c_logger::get().log("[c_dataframe]: Duplicate column name: " + name + "\n", LOG_CLR_RED);
			return false;
		}

		return true;
	}

public:

	c_dataframe() = default;

	explicit c_dataframe(const std::string& csv_path, bool header = true)
	{
		this->from_csv(csv_path, header);
	}

	~c_dataframe() {}

	bool add_column(const std::string& name, const std::vector<value_t>& values)
	{
		return this->_append_column(name, values);
	}

	template<class t>
	bool add_column(const std::string& name, const std::vector<t>& values);

	template<class t>
	bool add_column(const std::string& name, std::vector<t>&& values);

	bool remove_column(const std::string& name)
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return false;
		}

		const size_t erased = this->m_data.erase(name);
		if (erased && this->m_data.empty()) { this->m_rows = 0; }

		return erased != 0;
	}

	bool remove_column(const std::vector<std::string>& names)
	{
		if (names.empty())
		{
			c_logger::get().log("[c_dataframe]: No column names\n", LOG_CLR_RED);
			return false;
		}

		for (const auto& k : names)
		{
			this->m_data.erase(k);
		}

		if (this->m_data.empty()) { this->m_rows = 0; }

		return true;
	}

	bool rename_column(const std::string& old_name, const std::string& new_name)
	{
		if (old_name.empty() || new_name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return false;
		}

		auto it = m_data.find(old_name);
		if (it == m_data.end())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return false;
		}

		if (m_data.find(new_name) != m_data.end())
		{
			c_logger::get().log("[c_dataframe]: Duplicate column name: " + new_name + "\n", LOG_CLR_RED);
			return false;
		}

		m_data.emplace(new_name, std::move(it->second));
		m_data.erase(it);

		return true;
	}

	bool dropf()
	{
		if (!this->m_rows) { return false; }

		for (auto& [name, col] : this->m_data) { col.erase(col.begin()); }
		this->m_rows--;

		return true;
	}

	bool dropb()
	{
		if (!this->m_rows) { return false; }

		for (auto& [name, col] : this->m_data) { col.pop_back(); }
		this->m_rows--;

		return true;
	}

	bool drop(size_t index)
	{
		if (!this->m_rows || index >= this->m_rows) { return false; }

		for (auto& [name, col] : this->m_data) { col.erase(col.begin() + index); }
		this->m_rows--;

		return true;
	}

	bool dropna()
	{
		if (!this->m_rows) { return false; }

		auto is_str_nan = [](const std::string& s) -> bool {
			if (s.size() != 3) { return false; }
			auto tl = [](unsigned char c) { return static_cast<char>(std::tolower(c)); };
			return tl(s[0]) == 'n' && tl(s[1]) == 'a' && tl(s[2]) == 'n';
			};

		std::vector<size_t> rows_to_drop;
		rows_to_drop.reserve(this->m_rows);

		for (size_t i = 0; i < this->m_rows; ++i)
		{
			bool has_nan = false;
			for (auto& [name, col] : m_data)
			{
				if (std::holds_alternative<double>(col[i]))
				{
					double v = std::get<double>(col[i]);
					if (std::isnan(v))
					{
						has_nan = true;
						break;
					}
				}
				else
				{
					const std::string& v = std::get<std::string>(col[i]);
					if (is_str_nan(v)) { has_nan = true; break; }
				}
			}
			if (has_nan) { rows_to_drop.push_back(i); }
		}

		if (rows_to_drop.empty()) { return false; }

		std::sort(rows_to_drop.begin(), rows_to_drop.end());
		rows_to_drop.erase(std::unique(rows_to_drop.begin(), rows_to_drop.end()), rows_to_drop.end());

		for (auto it = rows_to_drop.rbegin(); it != rows_to_drop.rend(); ++it)
		{
			if (!this->drop(*it)) { return false; }
		}

		return true;
	}

	bool dropinf()
	{
		if (!this->m_rows) { return false; }

		auto is_str_inf = [](const std::string& s) -> bool {
			if (s.size() < 3 || s.size() > 4) { return false; }
			auto tl = [](unsigned char c) { return static_cast<char>(std::tolower(c)); };
			size_t start = 0;
			if (s[0] == '+' || s[0] == '-') { start = 1; }
			if (s.size() - start != 3) { return false; }
			return tl(s[start]) == 'i' && tl(s[start + 1]) == 'n' && tl(s[start + 2]) == 'f';
			};

		std::vector<size_t> rows_to_drop;
		rows_to_drop.reserve(this->m_rows);

		for (size_t i = 0; i < this->m_rows; ++i)
		{
			bool has_inf = false;
			for (auto& [name, col] : m_data)
			{
				if (std::holds_alternative<double>(col[i]))
				{
					double v = std::get<double>(col[i]);
					if (std::isinf(v))
					{
						has_inf = true;
						break;
					}
				}
				else
				{
					const std::string& v = std::get<std::string>(col[i]);
					if (is_str_inf(v)) { has_inf = true; break; }
				}
			}
			if (has_inf) { rows_to_drop.push_back(i); }
		}

		if (rows_to_drop.empty()) { return false; }

		std::sort(rows_to_drop.begin(), rows_to_drop.end());
		rows_to_drop.erase(std::unique(rows_to_drop.begin(), rows_to_drop.end()), rows_to_drop.end());

		for (auto it = rows_to_drop.rbegin(); it != rows_to_drop.rend(); ++it)
		{
			if (!this->drop(*it)) { return false; }
		}

		return true;
	}

	bool dropemp()
	{
		if (!this->m_rows) { return false; }

		std::vector<size_t> rows_to_drop;
		rows_to_drop.reserve(this->m_rows);

		for (size_t i = 0; i < this->m_rows; ++i)
		{
			bool has_empty = false;
			for (auto& [name, col] : m_data)
			{
				if (std::holds_alternative<std::string>(col[i]))
				{
					const std::string& v = std::get<std::string>(col[i]);
					if (_is_blank(v))
					{
						has_empty = true;
						break;
					}
				}
			}
			if (has_empty) { rows_to_drop.push_back(i); }
		}

		if (rows_to_drop.empty()) { return false; }

		std::sort(rows_to_drop.begin(), rows_to_drop.end());
		rows_to_drop.erase(std::unique(rows_to_drop.begin(), rows_to_drop.end()), rows_to_drop.end());

		for (auto it = rows_to_drop.rbegin(); it != rows_to_drop.rend(); ++it)
		{
			if (!this->drop(*it)) { return false; }
		}

		return true;
	}

	std::vector<value_t>& at(const std::string& name)
	{
		auto it = this->m_data.find(name);
		if (it != this->m_data.end()) { return it->second; }

		std::vector<value_t> v;
		v.resize(this->m_rows, value_t{std::string{}});

		auto [ins, ok] = this->m_data.emplace(name, std::move(v));
		return ins->second;
	}

	template<typename t>
	std::vector<t> at(const std::string& name);

	template<typename t>
	std::vector<std::vector<t>> at(const std::vector<std::string>& names);

	bool from_csv(const std::string& csv_path, bool header = true)
	{
		if (csv_path.empty()) { return false; }

		if (this->m_data.size() || this->m_rows)
		{
			c_logger::get().log("[c_dataframe]: Already initialized\n", LOG_CLR_RED);
			return false;
		}

		std::ifstream file(csv_path);
		if (!file.is_open()) { return false; }

		std::vector<std::vector<std::string>> values;

		std::string line;
		while (std::getline(file, line))
		{
			std::vector<std::string> fields = _parse_csv_line(line);
			values.resize(fields.size());
			for (int i = 0; i < fields.size(); ++i) { values[i].push_back(fields[i]); }
		}

		if (header)
		{
			this->m_rows = values.back().size() - 1;

			for (size_t i = 0; i < values.size(); ++i)
			{
				const std::string name = values[i][0];

				for (size_t j = 1; j < values[i].size(); ++j)
				{
					this->m_data[name].push_back(_to_value(values[i][j]));
				}
			}
		}
		else
		{
			this->m_rows = values.back().size();

			for (size_t i = 0; i < values.size(); ++i)
			{
				const std::string name = "col_" + std::to_string(i);

				for (size_t j = 0; j < values[i].size(); ++j)
				{
					this->m_data[name].push_back(_to_value(values[i][j]));
				}
			}
		}

		return true;
	}

	bool to_csv(const std::string& csv_path, bool header = true) const
	{
		if (csv_path.empty() || this->m_data.empty() || !this->m_rows) { return false; }

		std::ofstream file(csv_path, std::ios::out);
		if (!file.is_open()) { return false; }

		if (header)
		{
			int i = 0;
			for (const auto& [key, value] : this->m_data)
			{
				file << key;
				if (i + 1 < this->m_data.size()) { file << ','; }
				i++;
			}
		}

		for (size_t i = 0; i < this->m_rows; ++i)
		{
			if (header) { file << "\n"; }

			int j = 0;
			for (const auto& [key, value] : this->m_data)
			{
				const value_t& v = value[i];
				file << _value_to_string(v);
				if (j + 1 < this->m_data.size()) { file << ','; }
				j++;
			}

			if (!header) { file << "\n"; }
		}

		return true;
	}

	std::pair<size_t, size_t> shape() const
	{
		return { this->m_rows, this->m_data.size() };
	}

	double sum(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return std::numeric_limits<double>::quiet_NaN();
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return std::numeric_limits<double>::quiet_NaN(); }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		double sum = 0.0;
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				sum += d;
			}
		}

		return sum;
	}

	double prod(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return std::numeric_limits<double>::quiet_NaN();
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return std::numeric_limits<double>::quiet_NaN(); }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		double prod = 1.0;
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				prod *= d;
			}
		}

		return prod;
	}

	double mean(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return std::numeric_limits<double>::quiet_NaN();
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return std::numeric_limits<double>::quiet_NaN(); }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		double mean = 0.0;
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				mean += d;
			}
		}

		return mean / static_cast<double>(length);
	}

	double var(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return std::numeric_limits<double>::quiet_NaN();
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return std::numeric_limits<double>::quiet_NaN(); }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		if (length < 2) { return 0.0; }

		double mean = 0.0;
		double m2 = 0.0;
		size_t k = 0;
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				++k;
				const double delta = d - mean;
				mean += delta / static_cast<double>(k);
				m2 += delta * (d - mean);
			}
		}

		return m2 / static_cast<double>(length - 1);
	}

	double std(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		double v = this->var(name, from, length);
		return std::isnan(v) ? v : std::sqrt(v);
	}

	double min(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return std::numeric_limits<double>::quiet_NaN();
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return std::numeric_limits<double>::quiet_NaN(); }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		double min = std::numeric_limits<double>::max();
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				if (d < min) { min = d; }
			}
		}

		return min;
	}

	double max(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return std::numeric_limits<double>::quiet_NaN();
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return std::numeric_limits<double>::quiet_NaN(); }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		double max = std::numeric_limits<double>::min();
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				if (d > max) { max = d; }
			}
		}

		return max;
	}

	double cov(const std::string& name1, const std::string& name2, size_t from = 0, size_t length = 0) const
	{
		if (name1.empty() || name2.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return std::numeric_limits<double>::quiet_NaN();
		}

		auto it1 = this->m_data.find(name1);
		if (it1 == this->m_data.end()) { return std::numeric_limits<double>::quiet_NaN(); }

		auto it2 = this->m_data.find(name2);
		if (it2 == this->m_data.end()) { return std::numeric_limits<double>::quiet_NaN(); }

		const auto& col1 = it1->second;
		const auto& col2 = it2->second;

		const size_t n = col1.size();
		if (n == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		double mean1 = this->mean(name1, from, length);
		double mean2 = this->mean(name2, from, length);
		double covariance = 0.0;
		for (size_t i = from; i < from + length; ++i)
		{
			double d1 = std::numeric_limits<double>::quiet_NaN();
			double d2 = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col1[i]))
			{
				d1 = std::get<double>(col1[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col1[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d1 = *parsed;
			}

			if (std::holds_alternative<double>(col2[i]))
			{
				d2 = std::get<double>(col2[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col2[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d2 = *parsed;
			}

			if (!std::isnan(d1) && !std::isnan(d2))
			{
				covariance += ((d1 - mean1) * (d2 - mean2));
			}
		}

		return covariance / (static_cast<double>(length) - 1);
	}

	double corr(const std::string& name1, const std::string& name2, size_t from = 0, size_t length = 0) const
	{
		if (name1.empty() || name2.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return std::numeric_limits<double>::quiet_NaN();
		}

		double covariance = this->cov(name1, name2, from, length);
		if (std::isnan(covariance)) { return std::numeric_limits<double>::quiet_NaN(); }

		double std1 = this->std(name1, from, length);
		if (std::isnan(std1)) { return std::numeric_limits<double>::quiet_NaN(); }

		double std2 = this->std(name2, from, length);
		if (std::isnan(std2)) { return std::numeric_limits<double>::quiet_NaN(); }

		return covariance / (std1 * std2);
	}

	double skew(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return std::numeric_limits<double>::quiet_NaN();
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return std::numeric_limits<double>::quiet_NaN(); }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		double mean = this->mean(name, from, length);
		double std = this->std(name, from, length);
		double skewness = 0.0;
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				skewness += std::pow((d - mean) / std, 3.0);
			}
		}

		return skewness / static_cast<double>(length);
	}

	double kurt(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return std::numeric_limits<double>::quiet_NaN();
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return std::numeric_limits<double>::quiet_NaN(); }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return std::numeric_limits<double>::quiet_NaN(); }

		double mean = this->mean(name, from, length);
		double std = this->std(name, from, length);
		double kurtosis = 0.0;
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				kurtosis += std::pow((d - mean) / std, 4.0);
			}
		}

		return (kurtosis / static_cast<double>(length) - 3.0);
	}

	std::vector<double> diff(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return {};
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return {}; }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return {}; }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return {}; }

		std::vector<double> vals;
		vals.reserve(length);
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				vals.emplace_back(d);
			}
		}

		std::vector<double> diffs = n_math::first_diff(vals);
		diffs.insert(diffs.begin(), std::numeric_limits<double>::quiet_NaN());

		return diffs;
	}

	std::vector<double> diff(const std::string& name1, const std::string& name2, size_t from = 0, size_t length = 0) const
	{
		if (name1.empty() || name2.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return {};
		}

		auto it1 = this->m_data.find(name1);
		if (it1 == this->m_data.end()) { return {}; }

		auto it2 = this->m_data.find(name2);
		if (it2 == this->m_data.end()) { return {}; }

		const auto& col1 = it1->second;
		const auto& col2 = it2->second;

		const size_t n = col1.size();
		if (n == 0) { return {}; }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return {}; }

		std::vector<double> vals;
		vals.reserve(length);
		for (size_t i = from; i < from + length; ++i)
		{
			double d1 = std::numeric_limits<double>::quiet_NaN();
			double d2 = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col1[i]))
			{
				d1 = std::get<double>(col1[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col1[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d1 = *parsed;
			}

			if (std::holds_alternative<double>(col2[i]))
			{
				d2 = std::get<double>(col2[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col2[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d2 = *parsed;
			}

			if (!std::isnan(d1) && !std::isnan(d2))
			{
				vals.emplace_back(d1 - d2);
			}
		}

		return vals;
	}

	std::vector<double> pct_change(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return {};
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return {}; }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return {}; }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return {}; }

		std::vector<double> vals;
		vals.reserve(length);
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				vals.emplace_back(d);
			}
		}

		std::vector<double> changes = n_math::relative_change_vector(vals);
		changes.insert(changes.begin(), std::numeric_limits<double>::quiet_NaN());

		return changes;
	}

	std::vector<double> pct_change(const std::string& name1, const std::string& name2, size_t from = 0, size_t length = 0) const
	{
		if (name1.empty() || name2.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return {};
		}

		auto it1 = this->m_data.find(name1);
		if (it1 == this->m_data.end()) { return {}; }

		auto it2 = this->m_data.find(name2);
		if (it2 == this->m_data.end()) { return {}; }

		const auto& col1 = it1->second;
		const auto& col2 = it2->second;

		const size_t n = col1.size();
		if (n == 0) { return {}; }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return {}; }

		std::vector<double> vals;
		vals.reserve(length);
		for (size_t i = from; i < from + length; ++i)
		{
			double d1 = std::numeric_limits<double>::quiet_NaN();
			double d2 = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col1[i]))
			{
				d1 = std::get<double>(col1[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col1[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d1 = *parsed;
			}

			if (std::holds_alternative<double>(col2[i]))
			{
				d2 = std::get<double>(col2[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col2[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d2 = *parsed;
			}

			if (!std::isnan(d1) && !std::isnan(d2))
			{
				vals.emplace_back(n_math::relative_change(d1, d2));
			}
		}

		return vals;
	}

	std::vector<double> log_change(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return {};
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return {}; }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return {}; }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return {}; }

		std::vector<double> vals;
		vals.reserve(length);
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				vals.emplace_back(d);
			}
		}

		std::vector<double> changes = n_math::log_relative_change_vector(vals);
		changes.insert(changes.begin(), std::numeric_limits<double>::quiet_NaN());

		return changes;
	}

	std::vector<double> log_change(const std::string& name1, const std::string& name2, size_t from = 0, size_t length = 0) const
	{
		if (name1.empty() || name2.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return {};
		}

		auto it1 = this->m_data.find(name1);
		if (it1 == this->m_data.end()) { return {}; }

		auto it2 = this->m_data.find(name2);
		if (it2 == this->m_data.end()) { return {}; }

		const auto& col1 = it1->second;
		const auto& col2 = it2->second;

		const size_t n = col1.size();
		if (n == 0) { return {}; }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return {}; }

		std::vector<double> vals;
		vals.reserve(length);
		for (size_t i = from; i < from + length; ++i)
		{
			double d1 = std::numeric_limits<double>::quiet_NaN();
			double d2 = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col1[i]))
			{
				d1 = std::get<double>(col1[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col1[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d1 = *parsed;
			}

			if (std::holds_alternative<double>(col2[i]))
			{
				d2 = std::get<double>(col2[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col2[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d2 = *parsed;
			}

			if (!std::isnan(d1) && !std::isnan(d2))
			{
				vals.emplace_back(n_math::log_relative_change(d1, d2));
			}
		}

		return vals;
	}

	std::vector<double> cumsum(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return {};
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return {}; }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return {}; }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return {}; }

		std::vector<double> vals;
		vals.reserve(length);
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				vals.emplace_back(d);
			}
		}

		std::vector<double> cumsum;
		cumsum.reserve(length);
		cumsum.emplace_back(vals[0]);
		for (size_t i = 1; i < vals.size(); ++i) { cumsum.emplace_back(cumsum.back() + vals[i]); }

		return cumsum;
	}

	std::vector<double> cumprod(const std::string& name, size_t from = 0, size_t length = 0) const
	{
		if (name.empty())
		{
			c_logger::get().log("[c_dataframe]: No column name\n", LOG_CLR_RED);
			return {};
		}

		auto it = this->m_data.find(name);
		if (it == this->m_data.end()) { return {}; }

		const auto& col = it->second;

		const size_t n = col.size();
		if (n == 0) { return {}; }

		if (length == 0 || from + length > n) { length = n - from; }
		if (from >= n || length == 0) { return {}; }

		std::vector<double> vals;
		vals.reserve(length);
		for (size_t i = from; i < from + length; ++i)
		{
			double d = std::numeric_limits<double>::quiet_NaN();

			if (std::holds_alternative<double>(col[i]))
			{
				d = std::get<double>(col[i]);
			}
			else
			{
				const std::string& s = std::get<std::string>(col[i]);
				if (_is_blank(s)) { continue; }
				auto parsed = _parse_double(s);
				if (!parsed.has_value()) { continue; }
				d = *parsed;
			}

			if (!std::isnan(d))
			{
				vals.emplace_back(d);
			}
		}

		std::vector<double> cumprod;
		cumprod.reserve(length);
		cumprod.emplace_back(vals[0]);
		for (size_t i = 1; i < vals.size(); ++i) { cumprod.emplace_back(cumprod.back() * vals[i]); }

		return cumprod;
	}

	void reset()
	{
		this->m_data.clear();
		this->m_rows = 0;
	}

	c_dataframe head(size_t n = 5) const
	{
		if (this->m_data.empty() || !this->m_rows || !n)
		{
			c_logger::get().log("[c_dataframe]: No data\n", LOG_CLR_RED);
			return c_dataframe();
		}

		c_dataframe df;
		size_t num = std::min(n, this->m_rows);

		for (const auto& [name, col] : this->m_data)
		{
			std::vector<value_t> sub(col.begin(), col.begin() + num);
			df.m_data.emplace(name, std::move(sub));
		}

		df.m_rows = num;
		return df;
	}

	c_dataframe tail(size_t n = 5) const
	{
		if (this->m_data.empty() || !this->m_rows || !n)
		{
			c_logger::get().log("[c_dataframe]: No data\n", LOG_CLR_RED);
			return c_dataframe();
		}

		c_dataframe df;
		size_t num = std::min(n, this->m_rows);

		for (const auto& [name, col] : this->m_data)
		{
			std::vector<value_t> sub(col.end() - num, col.end());
			df.m_data.emplace(name, std::move(sub));
		}

		df.m_rows = num;
		return df;
	}

	void print(size_t n = 5) const
	{
		if (this->m_data.empty() || !this->m_rows || !n)
		{
			c_logger::get().log("[c_dataframe]: No data\n", LOG_CLR_RED);
			return;
		}

		c_logger::get().log_formatted("\t", LOG_CLR_NORMAL, 30);
		for (const auto& [key, value] : this->m_data) { c_logger::get().log_formatted(key, LOG_CLR_YELLOW, 30); }
		std::cout << std::endl << std::endl;

		size_t num = std::min(n, this->m_rows);
		for (size_t i = 0; i < num; ++i)
		{
			c_logger::get().log_formatted("\t" + std::to_string(i), LOG_CLR_YELLOW, 30);
			for (const auto& [key, value] : this->m_data)
			{
				c_logger::get().log_formatted(_value_to_string(value[i]), LOG_CLR_NORMAL, 30);
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};

template<size_t n>
inline value_t c_dataframe::_to_value(const char(&lit)[n])
{
	return value_t{ std::string(lit) };
}

template<class t>
std::vector<value_t> c_dataframe::_to_column(const std::vector<t>& v)
{
	std::vector<value_t> out;
	out.reserve(v.size());
	for (const auto& x : v) { out.push_back(_to_value(x)); }
	return out;
}

template<class t>
std::vector<value_t> c_dataframe::_to_column(std::vector<t>&& v)
{
	std::vector<value_t> out;
	out.reserve(v.size());
	for (auto& x : v) { out.push_back(_to_value(std::move(x))); }
	return out;
}


template<class t>
bool c_dataframe::add_column(const std::string& name, const std::vector<t>& values)
{
	return this->_append_column(name, _to_column(values));
}

template<class t>
bool c_dataframe::add_column(const std::string& name, std::vector<t>&& values)
{
	return this->_append_column(name, _to_column(std::move(values)));
}

template<typename t>
std::vector<t> c_dataframe::at(const std::string& name)
{
	const auto& col = this->m_data.at(name);
	std::vector<t> out;
	out.reserve(col.size());

	if (std::holds_alternative<double>(col[0]))
	{
		if constexpr (std::is_same_v<t, double>)
		{
			for (const auto& v : col) { out.push_back(std::get<double>(v)); }
		}
		else if constexpr (std::is_integral_v<t> || std::is_same_v<t, bool>)
		{
			for (const auto& v : col) { out.push_back(static_cast<t>(std::get<double>(v))); }
		}
		else
		{
			throw std::runtime_error("c_dataframe::at<T>: requested non-numeric T from numeric column");
		}

		return out;
	}

	if constexpr (std::is_same_v<t, std::string>)
	{
		for (const auto& v : col) { out.push_back(std::get<std::string>(v)); }
		return out;
	}
	else if constexpr (std::is_same_v<t, double>)
	{
		for (const auto& v : col)
		{
			const auto& s = std::get<std::string>(v);
			if (_is_blank(s))
			{
				out.push_back(std::numeric_limits<double>::quiet_NaN());
				continue;
			}
			auto parsed = _parse_double(s);
			out.push_back(parsed.value_or(std::numeric_limits<double>::quiet_NaN()));
		}
		return out;
	}
	else if constexpr (std::is_integral_v<t> || std::is_same_v<t, bool>)
	{
		for (const auto& v : col)
		{
			const auto& s = std::get<std::string>(v);
			auto parsed = _parse_double(s);
			if (parsed.has_value() && std::isfinite(*parsed)) { out.push_back(static_cast<t>(*parsed)); }
			else { throw std::runtime_error("c_dataframe::at<T>: non-parsable string to integer"); }
		}
		return out;
	}
	else { throw std::runtime_error("c_dataframe::at<T>: unsupported T for string column"); }
}

template<typename t>
inline std::vector<std::vector<t>> c_dataframe::at(const std::vector<std::string>& names)
{
	if (names.empty())
	{
		c_logger::get().log("[c_dataframe]: No column names\n", LOG_CLR_RED);
		return {};
	}

	std::vector<std::vector<t>> out;
	out.reserve(names.size());

	for (const std::string& name : names)
	{
		out.emplace_back(this->at<t>(name));
	}

	return out;
}