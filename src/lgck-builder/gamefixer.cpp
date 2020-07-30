#include "gamefixer.h"
#include "Game.h"
#include "FileWrap.h"
#include "vlamits3.h"
#include "Level.h"
#include "Layer.h"
#include <vector>

CGameFixer::CGameFixer()
{
    // https://publicdomainvectors.org/en/search/light/1
    m_game = nullptr;
    m_severity = Okay;
}

void CGameFixer::setGame(CGame *game)
{
    ASSERT(game);
    m_game = game;
}

void CGameFixer::addSampleSprites()
{
    typedef struct {
        std::string uuid;
        int id;
    } Sprite;

    std::unordered_map<std::string, Sprite> sprites;
    typedef struct {
        const char * name;
        const char * chToProto;
    } metaItem;

    metaItem items[] = {
        {
            "annie",
            nullptr
        },
        {
            "little_star (dead)",
            nullptr
        },
        {
            "little_star (player bullet)",
            "little_star (dead)"
        },
        {
            "sparkle",
            nullptr
        },
        {
            "diamond",
            "sparkle"
        },
        {
            "wallbrick",
            nullptr
        },
        {
            "drago",
            "sparkle"
        },
        {
            "deico",
            "sparkle"
        },
        {
            "switch94",
            nullptr
        },
        {
            "forcef94",
            nullptr
        },
        {
            "snake",
            "sparkle"
        },
    };

    CFileWrap file;
    for (unsigned int i=0; i < sizeof(items)/sizeof(metaItem); ++i) {
        CFrameSet *frameSet = new CFrameSet;
        int frameSetId = 0;
        metaItem & item = items[i];
        char *oblPath = nullptr;
        asprintf(&oblPath, ":res/meta/%s.obl", item.name);
        if (file.open(oblPath, "rb")) {
            frameSet->read(file);
            frameSet->setName(item.name);
            frameSetId = m_game->addFrameSet(frameSet);
            file.close();
        } else {
            qWarning("failed to read: %s", oblPath);
        }
        free(oblPath);

        char *metaPath = nullptr;
        asprintf(&metaPath, ":res/meta/%s.proto", item.name);
        if (file.open(metaPath, "rb")) {
            m_game->m_arrProto.getSize();
            CProtoArray t;
            t.importMeta(file);
            CProto & proto = t[0];
            proto.m_nFrameSet = frameSetId;
            proto.m_nChProto = item.chToProto ? sprites[item.chToProto].id : 0;
            proto.m_nChSound = 0;
            proto.m_nAutoProto = 0;
            proto.m_nAutoSound = 0;
            proto.m_nAutoBullet = 0;
            proto.m_nProtoBuddy = 0;
            proto.m_bulletSound = 0;
            strcpy(proto.m_szName, item.name);
            int id = m_game->m_arrProto.getSize();
            sprites[item.name] = Sprite {proto.m_uuid, id};
            m_game->m_arrProto += t;
            file.close();
        } else {
            qWarning("failed to read: %s", metaPath);
        }
        free(metaPath);
    }
}

void CGameFixer::troubleshoot()
{
    m_severity = Okay;
    m_errors.clear();

    if (!m_game->getSize()) {
        m_errors.push_back(
                    DesignError{
                        Error,
                        tr("No Level in your project. You must include one."),
                        tr("This is mandatory. Please go to Build, Create New Level to add a level."),
                        "/lgck/create_level.html"
                    });
    } else {
        m_errors.push_back(
                    DesignError{
                        Okay,
                        tr("Found at least 1 level in project."),
                        "",
                        ""
                    });
        CLayer * layer = m_game->getCurrentLevel().getMainLayer();
        if (!layer->getSize()) {
            m_errors.push_back(
                        DesignError{
                            Error,
                            tr("Current Level is empty. "),
                            tr("This is not intentional. Your level cannot be empty."),
                            "/lgck/edit_level.html"
                        });
        } else if (layer->countSpriteOfClass(*m_game, CLASS_PLAYER_OBJECT)) {
            m_errors.push_back(
                        DesignError{
                            Okay,
                            tr("Found player on current Level."),
                            "",
                            ""
                        });
        } else {
            m_errors.push_back(
                        DesignError{
                            Error,
                            tr("No player found in the current Level. You must include one."),
                            tr("A player object marks the start position is required for the level to execute properly. "\
                               "This is mandatory and will lead to failure if absent."),
                            "/lgck/sprite_wizard.html"
                        });
        }
        if (layer->countGoals()) {
            m_errors.push_back(
                        DesignError{
                            Okay,
                            tr("Current Level contains at least one goal."),
                            "",
                            ""
                        });
        } else {
            m_errors.push_back(
                        DesignError{
                            Warning,
                            tr("Current Level doesn't contains one goal."),
                            tr("Not including one goal on this level will lead to "\
                               "automatically completing the level upon entry."\
                               "This is likely not the intended result."),
                            ""
                        });
        }
    }

    if (!m_game->protos().countSpriteOfGivenClass(CLASS_PLAYER_OBJECT)) {
        m_errors.push_back(
                    DesignError{
                        Error,
                        tr("No Player Sprite in your project"),
                        tr("You need to supply a Player Object in your project. "\
                        "This is a mandatory requirement because no game can work without one. "),
                        ""
                    });
    } else {
        m_errors.push_back(
                    DesignError{
                        Okay,
                        tr("Found a Player Sprite in project"),
                        "",
                        ""
                    });
    }

    if (m_game->protos().getSize() == 1) {
        m_errors.push_back(
            DesignError{
                Error,
                tr("No Sprite in your project"),
                tr("You must supply a number of sprites including a Player Object to your project. "\
                "This is a mandatory requirement. Select File and New Project and The Wizard will fix this issue by including sample sprites. "),
                ""
            });
    } else if (m_game->protos().countAutoGoals()==0){
        m_errors.push_back(
            DesignError{
                Warning,
                tr("No sprite defined as an automatic goal."),
                tr("While This can be perfectly valid, "\
                   "you may be required to mark every single goal on the level manually. "\
                   "This is not recommended."),
                ""
        });
    }

    for(auto error: m_errors){
        m_severity = m_severity > error.severity ? m_severity : error.severity;
    }
}

const char *CGameFixer::getIcon(Severity severity)
{
    switch (severity) {
        case Okay:
            return ":/images/Light-bulb-green.png";
        case Warning:
            return ":/images/Light-bulb-yellow.png";
        case Error:
            return ":/images/Light-bulb-red.png";
    }
    return "";
}

const char *CGameFixer::getIcon()
{
    return getIcon(m_severity);
}

void CGameFixer::getStatus(QString & status)
{
    int warnings = 0;
    int errors = 0;

    for(auto error: m_errors){
        if (error.severity == Warning) {
            ++ warnings;
        } else if (error.severity == Error) {
            ++ errors;
        }
    }
    if (errors) {
        status = tr("%1 error%2 detected.").arg(errors).arg(errors>1? "s" :"");
    } else if (warnings) {
        status = tr("%1 warning%2 detected.").arg(warnings).arg(warnings>1? "s" :"");
    } else {
        status = tr("No errors detected");
    }
}

QString CGameFixer::getText()
{
    QString out;
    for(auto error: m_errors){
        QString sError = tr("Info");
        QString img = tr("<img src='%1' width=32 height=32>").arg(getIcon(error.severity));
        if (error.severity == Warning) {
            sError = tr("Warning");
        } else if (error.severity == Error) {
            sError = tr("Error");
        }
        out += tr("<div><b>[%1] %2</b></div><div>%3<br><br></div>").arg(
                    img,
                    error.message,
                    error.severity != Okay ? error.tip : tr("This requirement is satisfied."));
    }
    return out;
}
