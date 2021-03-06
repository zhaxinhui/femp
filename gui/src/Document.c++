#include "Document.h++"

#include <QTextStream>

#include <libfemp/Model.h++>
#include <libfemp/parsers/fem_msh.h++>
#include <libfemp/parsers/MshParser.h++>
#include <libfemp/export/JsonExporter.h++>


Document::Document()
{
    m_unsaved = false;
    m_filename = nullptr;

    m_documentType = TYPE_SOLID3D;	// nasty hack due to poor design
}


Document::Document(const Document & copied)
{
    this->m_unsaved = copied.m_unsaved;

    this->m_filename = copied.m_filename;
    this->m_documentType = copied.m_documentType;
}


Document::~Document()
{
}


void
Document::clear()
{
    m_unsaved = false;
    m_documentType = TYPE_NONE;
    clearFileName();
    this->m_project.clear();
}


void
Document::setFileName(QString new_file)
{
    this->m_filename = new_file;

    // check if file exists
    QFile           file;
    file.setFileName(m_filename);
}


void
Document::clearFileName()
{
    m_filename.clear();
}


QString
Document::getFileName() const
{
    return m_filename;
}

bool
Document::isFileNameNotSet() const
{
    return m_filename.isEmpty();
}


void
Document::setProjectType(Document::Type type)
{
    m_documentType = type;
}


fem::Project &
Document::getProject()
{
    return this->m_project;
}


void
Document::setUnsaved(bool unsaved)
{
    m_unsaved = unsaved;
}

