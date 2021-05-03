/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/MongoCursor.d.ts" />
/**
 * @description mongodb 数据库数据集对象
 * 
 *  使用 MongoDB.getCollection 创建：
 *  ```JavaScript
 *  var col1 = mdb.getCollection('test');
 *  var col = mdb.test;
 *  ```
 *  
 */
declare class Class_MongoCollection extends Class_object {
    /**
     * @description 根据给定的查询条件和返回字段设定，建立游标对象
     *     @param query 指定查询条件的对象
     *     @param projection 指定返回字段的对象
     *     @return 返回游标对象
     *    
     */
    find(): Class_MongoCursor;

    /**
     * @description 根据给定的查询条件和返回字段设定，查询一条结果
     *     @param query 指定查询条件的对象
     *     @param projection 指定返回字段的对象
     *     @return 返回第一条结果
     *    
     */
    findOne(): object;

    /**
     * @description 查询并修改
     *     @param query 指定查询条件，修改数据
     *     @return 返回修改前的结果及其他信息
     *    
     */
    findAndModify(): object;

    /**
     * @description 插入一组数据
     *     @param documents 指定要插入的数据数组
     *    
     */
    insert(): void;

    /**
     * @description 保存一条数据，若数据包含 _id 字段，则为更新，否则为插入
     *     @param document 指定要保存的数据
     *    
     */
    save(): void;

    /**
     * @description 根据给定的查询条件更新数据
     *     @param query 指定查询条件的对象
     *     @param document 指定要更新的数据
     *     @param upsert 数据不存在时，插入一条新数据，缺省为 false，不插入
     *     @param multi 当符合条件的数据多于一条时，更新所有数据，缺省为 false，只更新第一条
     *    
     */
    update(): void;

    /**
     * @description 根据给定的查询条件删除数据
     *     @param query 指定查询条件的对象
     *    
     */
    remove(): void;

    /**
     * @description 执行数据库命令
     *     @param cmd 给定命令对象
     *     @return 返回命令返回结果
     *    
     */
    runCommand(): object;

    /**
     * @description 删除当前集合 
     */
    drop(): void;

    /**
     * @description 在当前集合上创建索引
     *     @param keys 给定索引字段、顺序和方向
     *     @param options 给定索引的选项，唯一索引等
     *    
     */
    ensureIndex(): void;

    /**
     * @description 重建当前集合的索引
     *     @return 返回命令执行结果
     *    
     */
    reIndex(): object;

    /**
     * @description 删除当前集合指定名称的索引
     *     @param name 给定要删除的索引名称
     *     @return 返回命令执行结果
     *    
     */
    dropIndex(): object;

    /**
     * @description 删除当前集合全部索引
     *     @return 返回命令执行结果
     *    
     */
    dropIndexes(): object;

    /**
     * @description 查询当前集合全部索引
     *     @return 返回包含索引的结果集
     *    
     */
    getIndexes(): any[];

    /**
     * @description 获取当前集合子命名空间的集合对象
     *     @param name 子命名空间名称
     *     @return 返回新集合对象
     *    
     */
    getCollection(): Class_MongoCollection;


}
