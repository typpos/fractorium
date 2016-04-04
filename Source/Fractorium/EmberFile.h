#pragma once

#include "FractoriumCommon.h"

/// <summary>
/// EmberFile class.
/// </summary>

/// <summary>
/// Class for representing an ember Xml file in memory.
/// It contains a filename and a vector of embers.
/// It also provides static helper functions for creating
/// default names for the file and the embers in it.
/// </summary>
template <typename T>
class EmberFile
{
public:
	/// <summary>
	/// Default constructor and destructor.
	/// </summary>
	EmberFile() = default;
	~EmberFile() = default;

	/// <summary>
	/// Default copy constructor.
	/// </summary>
	/// <param name="emberFile">The EmberFile object to copy</param>
	EmberFile(const EmberFile<T>& emberFile)
	{
		EmberFile<T>::operator=<T>(emberFile);
	}

	/// <summary>
	/// Copy constructor to copy an EmberFile object of type U.
	/// </summary>
	/// <param name="emberFile">The EmberFile object to copy</param>
	template <typename U>
	EmberFile(const EmberFile<U>& emberFile)
	{
		EmberFile<T>::operator=<U>(emberFile);
	}

	/// <summary>
	/// Default assignment operator.
	/// </summary>
	/// <param name="emberFile">The EmberFile object to copy</param>
	EmberFile<T>& operator = (const EmberFile<T>& emberFile)
	{
		if (this != &emberFile)
			EmberFile<T>::operator=<T>(emberFile);

		return *this;
	}

	/// <summary>
	/// Assignment operator to assign a EmberFile object of type U.
	/// </summary>
	/// <param name="emberFile">The EmberFile object to copy.</param>
	/// <returns>Reference to updated self</returns>
	template <typename U>
	EmberFile<T>& operator = (const EmberFile<U>& emberFile)
	{
		m_Filename = emberFile.m_Filename;
		CopyCont(m_Embers, emberFile.m_Embers);
		return *this;
	}

	/// <summary>
	/// Move constructor.
	/// </summary>
	/// <param name="emberFile">The EmberFile object to move</param>
	EmberFile(EmberFile<T>&& emberFile)
	{
		EmberFile<T>::operator=<T>(emberFile);
	}

	/// <summary>
	/// Move assignment operator.
	/// </summary>
	/// <param name="emberFile">The EmberFile object to move</param>
	EmberFile<T>& operator = (EmberFile<T>&& emberFile)
	{
		if (this != &emberFile)
		{
			m_Filename = emberFile.m_Filename;
			m_Embers = std::move(emberFile.m_Embers);
		}

		return *this;
	}

	/// <summary>
	/// Clear the file name and the vector of embers.
	/// </summary>
	void Clear()
	{
		m_Filename.clear();
		m_Embers.clear();
	}

	/// <summary>
	/// Thin wrapper to get the size of the vector of embers.
	/// </summary>
	size_t Size()
	{
		return m_Embers.size();
	}

	/// <summary>
	/// Get a pointer to the ember at the specified index.
	/// </summary>
	/// <param name="i">The index of the ember to retrieve</param>
	/// <returns>A pointer to the ember if it was within bounds, else nullptr.</returns>
	Ember<T>* Get(size_t i)
	{
		if (i < m_Embers.size())
			return &(*Advance(m_Embers.begin(), i));

		return nullptr;
	}

	/// <summary>
	/// Delete the ember at the given index.
	/// Will not delete anything if the size is already 1.
	/// </summary>
	/// <param name="index">The index of the ember to delete</param>
	/// <returns>True if successfully deleted, else false.</returns>
	bool Delete(size_t index)
	{
		if (Size() > 1 && index < Size())
		{
			m_Embers.erase(Advance(m_Embers.begin(), index));
			return true;
		}
		else
			return false;
	}

	/// <summary>
	/// Ensure all ember names are unique.
	/// </summary>
	void MakeNamesUnique()
	{
		for (auto it1 = m_Embers.begin(); it1 != m_Embers.end(); ++it1)
		{
			for (auto it2 = m_Embers.begin(); it2 != m_Embers.end(); ++it2)
			{
				if (it1 != it2 && it1->m_Name == it2->m_Name)
				{
					it2->m_Name = IncrementTrailingUnderscoreInt(QString::fromStdString(it2->m_Name)).toStdString();
					it2 = m_Embers.begin();
				}
			}
		}
	}

	/// <summary>
	/// Return the default filename based on the current date/time.
	/// </summary>
	/// <returns>The default filename</returns>
	static QString DefaultFilename()
	{
		return "Flame_" + QDateTime(QDateTime::currentDateTime()).toString("yyyy-MM-dd-hhmmss");
	}

	/// <summary>
	/// Return a copy of the string which ends with _# where # is the
	/// previous number at that position incremented by one.
	/// If the original string did not end with _#, the returned
	/// string will just have _1 appended to it.
	/// </summary>
	/// <param name="str">The string to process</param>
	/// <returns>The original string with the number after the final _ character incremented by one</returns>
	static QString IncrementTrailingUnderscoreInt(const QString& str)
	{
		bool ok = false;
		size_t num = 0;
		QString endSection;
		QString ret = str;
		int lastUnderscore = str.lastIndexOf('_');

		if (lastUnderscore != -1)
		{
			endSection = str.section('_', -1);
			num = endSection.toULongLong(&ok);

			if (ok)
				ret.chop(str.size() - lastUnderscore);
		}

		ret += "_" + QString::number(num + 1);
		return ret;
	}

	/// <summary>
	/// Ensures a given input filename is unique by appending a count to the end.
	/// </summary>
	/// <param name="filename">The filename to ensure is unique</param>
	/// <returns>The passed in name if it was unique, else a uniquely made name.</returns>
	static QString UniqueFilename(const QString& filename)
	{
		if (!QFile::exists(filename))
			return filename;

		QString newPath;
		QFileInfo original(filename);
		QString path = original.absolutePath() + '/';
		QString base = original.completeBaseName();
		QString extension = original.suffix();

		do
		{
			base = IncrementTrailingUnderscoreInt(base);
			newPath = path + base + "." + extension;
		}
		while (QFile::exists(newPath));

		return newPath;
	}

	/// <summary>
	/// Return the default ember name based on the current date/time and
	/// the ember's index in the file.
	/// </summary>
	/// <param name="i">The index in the file of the ember</param>
	/// <returns>The default ember name</returns>
	static QString DefaultEmberName(T i)
	{
		return DefaultFilename() + "_" + ToString<T>(i);
	}

	QString m_Filename;
	list<Ember<T>> m_Embers;
};
