/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/XmlDocumentType.d.ts" />
/// <reference path="../interface/XmlElement.d.ts" />
/// <reference path="../interface/XmlNodeList.d.ts" />
/// <reference path="../interface/XmlText.d.ts" />
/// <reference path="../interface/XmlComment.d.ts" />
/// <reference path="../interface/XmlCDATASection.d.ts" />
/// <reference path="../interface/XmlProcessingInstruction.d.ts" />
/**
 * @description XmlDocument 是  xml 模块的一个对象，它代表整个 XML 文档，提供了对整个文档的访问入口
 * 
 * XmlDocument 是一棵文档树的根，包含了整个 XML 文档中的所有节点。XmlDocument 对象同样提供了以下功能：
 * 
 * 1. 创建元素节点、文本节点、注释和处理指令等
 * 2. 访问和修改文档属性和相关信息（如 DTD 注释及文档声明）
 * 3. 解析 XML 文档
 * 
 * 下面是使用 XmlDocument 对象解析一个 XML 文档的示例代码：
 * 
 * ```JavaScript
 * var xml = require('xml');
 * var fs = require('fs');
 * 
 * var xmlStr = fs.readFile('test.xml');
 * var xmlDoc = xml.parse(xmlStr);
 * 
 * // get document root node name
 * var rootName = xmlDoc.documentElement.nodeName;
 * console.log(`文档根节点名称是 ${rootName}`);
 * ```
 * 
 * 在上述代码中，我们首先使用 `fs` 模块的 `readFile()` 方法读取了一个 XML 文件，并将文件流赋值给变量 `xmlStr`。然后我们使用 `xml` 模块的 `parse()` 方法解析该 XML 文件，并将解析后的 `XmlDocument` 对象赋值给变量 `xmlDoc`。最后我们使用 `xmlDoc` 的 `documentElement` 属性获取文档根节点，并获取其节点名称，输出到控制台上。
 * 
 * 由于 XmlDocument 是整个 XML 文档的入口，因此我们可以通过它获取和修改文档的相关信息。例如，我们可以通过 `xmlDoc.xmlVersion` 和 `xmlDoc.xmlStandalone` 分别获取和修改文档的 XML 版本和 standalone 属性。我们还可以通过 `xmlDoc.createProcessingInstruction()` 方法来创建新的处理指令节点。
 * 
 * XmlDocument 对象是一个非常强大的类型，为我们处理解析 XML 文件提供了很大的便利。
 * 
 */
declare class Class_XmlDocument extends Class_XmlNode {
    /**
     * @description 构造一个 XmlDocument 对象
     *      @param type 指定文档对象的类型，缺省为 "text/xml"，若需要处理 html 则需要指定 "text/html"
     *     
     */
    constructor(type?: string);

    /**
     * @description 通过解析一个 XML/HTML 字符串来组成该文档，不支持多语种
     *      @param source 要解析的 XML/HTML 文本，取决于文档创建时的类型
     *     
     */
    load(source: string): void;

    /**
     * @description 通过解析一个二进制 XML/HTML 字符串来组成该文档，并根据语种自动转换
     *      @param source 要解析的 XML/HTML 文本，取决于文档创建时的类型
     *     
     */
    load(source: Class_Buffer): void;

    /**
     * @description 返回用于文档的编码（在解析时）
     *     
     */
    readonly inputEncoding: string;

    /**
     * @description 设置或返回文档是否为 standalone
     *     
     */
    xmlStandalone: boolean;

    /**
     * @description 设置或返回文档的 XML 版本
     *     
     */
    xmlVersion: string;

    /**
     * @description 返回与文档相关的文档类型声明（Document Type Declaration）
     * 
     *     对于没有 DTD 的 XML 文档，则返回 null。此属性可提供对 XmlDocumentType 对象（ XmlDocument 的一个子节点）的直接访问。
     *     
     */
    readonly doctype: Class_XmlDocumentType;

    /**
     * @description 返回文档的根节点
     *     
     */
    readonly documentElement: Class_XmlElement;

    /**
     * @description 返回 HTML 文档的 head 节点，仅在 html 模式有效
     *     
     */
    readonly head: Class_XmlElement;

    /**
     * @description 返回 HTML 文档的 title 节点的内容，仅在 html 模式有效
     *     
     */
    readonly title: string;

    /**
     * @description 返回 HTML 文档的 body 节点，仅在 html 模式有效
     *     
     */
    readonly body: Class_XmlElement;

    /**
     * @description 返回带有指定名称的所有元素的一个节点列表
     * 
     *      该方法将返回一个 XmlNodeList 对象（可以作为只读数组处理），该对象存放文档中具有指定标签名的所有 XmlElement 节点，它们存放的顺序就是在源文档中出现的顺序。 XmlNodeList 对象是“活”的，即如果在文档中添加或删除了指定标签名的元素，它的内容会自动进行必要的更新。
     *      @param tagName 需检索的标签名。值 "*" 匹配所有的标签
     *      @return 文档树中具有指定标记的 XmlElement 节点的 XmlNodeList 集合。返回的元素节点的顺序就是它们在源文档中出现的顺序。
     *     
     */
    getElementsByTagName(tagName: string): Class_XmlNodeList;

    /**
     * @description 返回带有指定命名空间和名称的所有元素的一个节点列表
     * 
     *      该方法与 getElementsByTagName() 方法相似，只是它根据命名空间和名称来检索元素。
     *      @param namespaceURI 指定检索的命名空间 URI。值 "*" 可匹配所有的标签
     *      @param localName 需检索的标签名。值 "*" 匹配所有的标签
     *      @return 文档树中具有指定标记的 XmlElement 节点的 XmlNodeList 集合。返回的元素节点的顺序就是它们在源文档中出现的顺序。
     *     
     */
    getElementsByTagNameNS(namespaceURI: string, localName: string): Class_XmlNodeList;

    /**
     * @description 返回拥有指定 id 属性的元素
     * 
     *      该方法将遍历文档的子孙节点，返回一个 XmlElement 节点对象，表示第一个具有指定 id 属性的文档元素。。
     *      @param id 需检索的 id
     *      @return 节点树中具有指定 id 属性的 XmlElement 节点
     *     
     */
    getElementById(id: string): Class_XmlElement;

    /**
     * @description 返回带有指定 class 名称的所有元素的一个节点列表
     * 
     *      该方法将返回一个 XmlNodeList 对象（可以作为只读数组处理），该对象存放文档中具有指定 class 名的所有 XmlElement 节点，它们存放的顺序就是在源文档中出现的顺序。 XmlNodeList 对象是“活”的，即如果在文档中添加或删除了指定标签名的元素，它的内容会自动进行必要的更新。
     *      @param className 需检索的 class 名称
     *      @return 文档树中具有指定 class 名的 XmlElement 节点的 XmlNodeList 集合。返回的元素节点的顺序就是它们在源文档中出现的顺序。
     *     
     */
    getElementsByClassName(className: string): Class_XmlNodeList;

    /**
     * @description 创建元素节点
     *      @param tagName 指定元素节点规定名称
     *      @return 返回新创建的 XmlElement 节点，具有指定的标签名
     *     
     */
    createElement(tagName: string): Class_XmlElement;

    /**
     * @description 创建带有指定命名空间的元素节点
     *      @param namespaceURI 指定元素节点命名空间 URI
     *      @param qualifiedName 指定元素节点规定名称
     *      @return 返回新创建的 XmlElement 节点，具有指定的标签名
     *     
     */
    createElementNS(namespaceURI: string, qualifiedName: string): Class_XmlElement;

    /**
     * @description 创建文本节点
     *      @param data 指定此节点的文本
     *      @return 返回新创建的 XmlText 节点，表示指定的 data 字符串
     *     
     */
    createTextNode(data: string): Class_XmlText;

    /**
     * @description 创建注释节点
     *      @param data 指定此节点的注释文本
     *      @return 返回新创建的 XmlComment 节点，注释文本为指定的 data
     *     
     */
    createComment(data: string): Class_XmlComment;

    /**
     * @description 创建 XmlCDATASection 节点
     *      @param data 指定此节点规定 CDATA 数据
     *      @return 返回新创建的 XmlCDATASection 节点，内容为指定的 data
     *     
     */
    createCDATASection(data: string): Class_XmlCDATASection;

    /**
     * @description 创建 XmlProcessingInstruction 节点
     *      @param target 指定处理指令的目标
     *      @param data 指定处理指令的内容文本
     *      @return 新创建的 ProcessingInstruction 节点
     *     
     */
    createProcessingInstruction(target: string, data: string): Class_XmlProcessingInstruction;

}

