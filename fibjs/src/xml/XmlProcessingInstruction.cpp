/*
 * XmlProcessingInstruction.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/xml.h"
#include "XmlProcessingInstruction.h"

namespace fibjs {

result_t XmlProcessingInstruction::get_nodeName(exlib::string& retVal)
{
    retVal = m_target;
    return 0;
}

result_t XmlProcessingInstruction::get_nodeValue(exlib::string& retVal)
{
    retVal = m_data;
    return 0;
}

result_t XmlProcessingInstruction::set_nodeValue(exlib::string newVal)
{
    m_data = newVal;
    return 0;
}

result_t XmlProcessingInstruction::get_nodeType(int32_t& retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlProcessingInstruction::get_parentNode(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_parentNode(retVal);
}

result_t XmlProcessingInstruction::get_childNodes(obj_ptr<XmlNodeList_base>& retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlProcessingInstruction::get_children(obj_ptr<XmlNodeList_base>& retVal)
{
    return XmlNodeImpl::get_children(retVal);
}

result_t XmlProcessingInstruction::get_firstChild(obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlProcessingInstruction::get_lastChild(obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlProcessingInstruction::get_previousSibling(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_previousSibling(retVal);
}

result_t XmlProcessingInstruction::get_nextSibling(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_nextSibling(retVal);
}

result_t XmlProcessingInstruction::get_firstElementChild(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_firstElementChild(retVal);
}

result_t XmlProcessingInstruction::get_lastElementChild(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_lastElementChild(retVal);
}

result_t XmlProcessingInstruction::get_previousElementSibling(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_previousElementSibling(retVal);
}

result_t XmlProcessingInstruction::get_nextElementSibling(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_nextElementSibling(retVal);
}

result_t XmlProcessingInstruction::get_textContent(exlib::string& retVal)
{
    return 0;
}

result_t XmlProcessingInstruction::set_textContent(exlib::string newVal)
{
    return 0;
}

result_t XmlProcessingInstruction::get_ownerDocument(obj_ptr<XmlDocument_base>& retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlProcessingInstruction::lookupPrefix(exlib::string namespaceURI, exlib::string& retVal)
{
    return XmlNodeImpl::lookupPrefix(namespaceURI, retVal);
}

result_t XmlProcessingInstruction::lookupNamespaceURI(exlib::string prefix, exlib::string& retVal)
{
    return XmlNodeImpl::lookupNamespaceURI(prefix, retVal);
}

result_t XmlProcessingInstruction::insertBefore(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlProcessingInstruction::insertAfter(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->insertAfter(newChild, refChild, retVal);
}

result_t XmlProcessingInstruction::replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlProcessingInstruction::removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlProcessingInstruction::appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->appendChild(newChild, retVal);
}

result_t XmlProcessingInstruction::hasChildNodes(bool& retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlProcessingInstruction::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlProcessingInstruction> pi = new XmlProcessingInstruction(*this);
    return XmlNodeImpl::cloneNode(pi, deep, retVal);
}

result_t XmlProcessingInstruction::normalize()
{
    return m_childs->normalize();
}

result_t XmlProcessingInstruction::get_target(exlib::string& retVal)
{
    retVal = m_target;
    return 0;
}

result_t XmlProcessingInstruction::get_data(exlib::string& retVal)
{
    retVal = m_data;
    return 0;
}

result_t XmlProcessingInstruction::set_data(exlib::string newVal)
{
    m_data = newVal;
    return 0;
}

result_t XmlProcessingInstruction::toString(exlib::string& retVal)
{
    retVal = "<?";
    retVal.append(m_target);
    retVal.append(1, ' ');
    retVal.append(m_data);
    retVal.append("?>");

    return 0;
}
}
