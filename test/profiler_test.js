var test = require("test");
test.setup();

var coroutine = require('coroutine');

function test_fiber() {
    coroutine.sleep(100);
}

var profiler = require("profiler");
var fs = require("fs");
var path = require("path");
var os = require("os");

var vmid = coroutine.vmid;

function unlink(pathname) {
    try {
        fs.unlink(pathname);
    } catch (e) { }
}

describe("profiler", () => {
    after(() => {
        unlink(path.join(__dirname, "test.heapsnapshot" + vmid));
        unlink(path.join(__dirname, "test1.heapsnapshot" + vmid));
        unlink(path.join(__dirname, "test2.heapsnapshot" + vmid));
    });

    it("take snapshot & dispose", () => {
        var ss = profiler.takeSnapshot();
    });

    it("crash when double take", () => {
        profiler.takeSnapshot();
        profiler.takeSnapshot();
    });

    it("serialize", () => {
        profiler.saveSnapshot(path.join(__dirname, "test.heapsnapshot" + vmid));

        var ss = profiler.loadSnapshot(path.join(__dirname, "test.heapsnapshot" + vmid));
        ss.save(path.join(__dirname, "test1.heapsnapshot" + vmid));

        var ss = profiler.loadSnapshot(path.join(__dirname, "test1.heapsnapshot" + vmid));
        ss.save(path.join(__dirname, "test2.heapsnapshot" + vmid));

        assert.equal(fs.readTextFile(path.join(__dirname, "test1.heapsnapshot" + vmid)),
            fs.readTextFile(path.join(__dirname, "test2.heapsnapshot" + vmid)));
    });

    it("diff", () => {
        var ss = profiler.loadSnapshot(path.join(__dirname, "test.heapsnapshot" + vmid));
        var ss1 = profiler.loadSnapshot(path.join(__dirname, "test.heapsnapshot" + vmid));

        assert.deepEqual(ss.diff(ss1).change, {
            "size_bytes": 0,
            "size": "0 bytes",
            "freed_nodes": 0,
            "allocated_nodes": 0,
            "details": []
        });
    });

    it("getHeapCodeStatistics", () => {
        var hcs = profiler.getHeapCodeStatistics();

        assert.property(hcs, "code_and_metadata_size");
        assert.property(hcs, "bytecode_and_metadata_size");
        assert.property(hcs, "external_script_source_size");
    });

    it("getHeapSpaceStatistics", () => {
        var hss = profiler.getHeapSpaceStatistics();

        assert.property(hss[0], "space_name");
        assert.property(hss[0], "space_size");
        assert.property(hss[0], "space_used_size");
        assert.property(hss[0], "space_available_size");
        assert.property(hss[0], "physical_space_size");
    });

    it("getHeapStatistics", () => {
        var hs = profiler.getHeapStatistics();

        assert.property(hs, "total_heap_size");
        assert.property(hs, "total_heap_size_executable");
        assert.property(hs, "total_physical_size");
        assert.property(hs, "total_available_size");
        assert.property(hs, "used_heap_size");
        assert.property(hs, "heap_size_limit");
        assert.property(hs, "malloced_memory");
        assert.property(hs, "external_memory");
        assert.property(hs, "peak_malloced_memory");
        assert.property(hs, "number_of_native_contexts");
        assert.property(hs, "number_of_detached_contexts");
    });

    it("Fiber.stack", () => {
        var fb = coroutine.start(test_fiber);
        coroutine.sleep(10);
        assert.equal(`    at test_fiber (${__filename}:7:15)`, fb.stack)
    });
});

require.main === module && test.run(console.DEBUG);