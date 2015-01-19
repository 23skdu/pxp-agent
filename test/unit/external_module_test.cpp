#include "test/test.h"

#include "src/data_container.h"
#include "src/errors.h"
#include "src/external_module.h"
#include "src/uuid.h"
#include "src/file_utils.h"

#include <boost/filesystem/operations.hpp>

#include <vector>

extern std::string ROOT_PATH;

namespace CthunAgent {

TEST_CASE("ExternalModule::ExternalModule", "[modules]") {
    SECTION("can successfully instantiate from a valid external module") {
        REQUIRE_NOTHROW(ExternalModule(
            ROOT_PATH + "/test/unit/test_modules/reverse_valid"));
    }

    SECTION("all actions are successfully loaded from a valid external module") {
        ExternalModule mod { ROOT_PATH + "/test/unit/test_modules/reverse_valid" };
        REQUIRE(mod.actions.size() == 2);
    }

    SECTION("throws an error in case of invalid overall metadata schema") {
        REQUIRE_THROWS_AS(
            ExternalModule(ROOT_PATH + "/test/unit/test_modules/reverse_broken_01"),
            module_error);
    }
}

static const std::string string_action { "string" };
static const std::string fake_action { "fake_action" };
static const std::string reverse_txt {
    "{\"data\" : {"
    "    \"module\" : \"reverse\","
    "    \"action\" : \"string\","
    "    \"params\" : \"maradona\""
    "    }"
    "}"
};
static const Message msg { reverse_txt };
static const std::string bad_reverse {
    "{\"data\" : {"
    "    \"module\" : \"reverse\","
    "    \"action\" : \"string\","
    "    \"params\" : [1, 2, 3, 4 ,5]"
    "    }"
    "}"
};

TEST_CASE("ExternalModule::validate_and_call_action", "[modules]") {
    ExternalModule reverse_module { ROOT_PATH + "/modules/reverse" };

    SECTION("it should correctly call the shipped reverse module") {
        auto result = reverse_module.validate_and_call_action(string_action, msg);
        REQUIRE(result.toString().find("anodaram") != std::string::npos);
    }

    SECTION("it should throw a validation_error if the action is unknown") {
        REQUIRE_THROWS_AS(reverse_module.validate_and_call_action(fake_action, msg),
                          validation_error);
    }

    SECTION("it should throw a validation_error if the message is invalid") {
        Message bad_msg { bad_reverse };
        REQUIRE_THROWS_AS(reverse_module.validate_and_call_action(string_action,
                                                                  bad_msg),
                          validation_error);
    }
}

TEST_CASE("ExternalModule::call_delayed_action", "[modules]") {
    ExternalModule reverse_module {
        ROOT_PATH + "/test/unit/test_modules/reverse_valid" };
    std::string action_parent_dir { "/tmp/cthun_agent/" };
    auto job_id = UUID::getUUID();
    auto action_dir = action_parent_dir + job_id;

    while (FileUtils::fileExists(action_dir)) {
        job_id =  UUID::getUUID();
        action_dir = action_parent_dir + job_id;
    }

    SECTION("it should create the result directory for a given job") {
        reverse_module.call_delayed_action(string_action, msg, job_id);
        CHECK(FileUtils::fileExists(action_dir));
        boost::filesystem::remove_all(action_dir);
    }

    SECTION("it should create the status, stdout, and stderr files") {
        reverse_module.call_delayed_action(string_action, msg, job_id);
        CHECK(FileUtils::fileExists(action_dir + "/status"));
        CHECK(FileUtils::fileExists(action_dir + "/stdout"));
        CHECK(FileUtils::fileExists(action_dir + "/stderr"));
        boost::filesystem::remove_all(action_dir);
    }

    SECTION("it should correctly write the completed status") {
        reverse_module.call_delayed_action(string_action, msg, job_id);
        CHECK(FileUtils::fileExists(action_dir + "/status"));

        if (FileUtils::fileExists(action_dir + "/status")) {
            auto result = FileUtils::readFileAsString(action_dir + "/status");
            CHECK(result.find("completed") != std::string::npos);
        }

        boost::filesystem::remove_all(action_dir);
    }

    SECTION("it should correctly write the result") {
        reverse_module.call_delayed_action(string_action, msg, job_id);
        CHECK(FileUtils::fileExists(action_dir + "/stdout"));

        if (FileUtils::fileExists(action_dir + "/stdout")) {
            auto result = FileUtils::readFileAsString(action_dir + "/stdout");
            CHECK(result.find("anodaram") != std::string::npos);
        }

        boost::filesystem::remove_all(action_dir);
    }
}

}  // namespace CthunAgent
