#pragma once

#include"oso_core.h"
#include <string>

namespace cdm {
    class SetConfig
    {
    public:
        SetConfig() = default;
        ~SetConfig() = default;

        SetConfig(const rw::oso::ObjectStoreAssembly& assembly);
        SetConfig(const SetConfig& obj);

        SetConfig& operator=(const SetConfig& obj);
        operator rw::oso::ObjectStoreAssembly() const;
        bool operator==(const SetConfig& obj) const;
        bool operator!=(const SetConfig& obj) const;

    public:
        double xiangjiguangdianpingbishijian{ 0 };
        double shuchuxinhaoyanshi{ 0 };
        double shuchuxinhaochixushijian{ 0 };
        int saveImgMode{ 0 };
        int score{ 0 };
        bool autoSaveImg{ false };
        double shangxianwei1{ 0 };
        double xiaxianwei1{ 0 };
        double zuoxianwei1{ 0 };
        double youxianwei1{ 0 };
        double xiangsudangliang1{ 0 };
        int debugFrame{ 0 };
        int openRemoveFrame{ 0 };
        double zengyi{ 0 };
        double qiangguang{ 0 };
        double zhongguang{ 0 };
        double ruoguang{ 0 };
        int shiceyahenkuanduduqudizhi{ 0 };
        int shedingyahenbiaozhunzhiduqudizhi{ 0 };
        int paizhaojiangejuliduqudizhi{ 0 };
        int daizichangdubiaozhunduqudizhi{ 0 };
        int jimiqiyiquanmaichongshuduqudizhi{ 0 };
        int jimiqiyiquanzhouchangduqudizhi{ 0 };
        int bujinyiquanmaichongshuduqudizhi{ 0 };
        int bujinluojuduqudizhi{ 0 };
        int daoyidongbuchangduqudizhi{ 0 };
        int zidongdaoyidongzuidajuliduqudizhi{ 0 };
        int daojiakeyidongdezuidajuliduqudizhi{ 0 };
        int daoyidongsudumaichongpinlvduqudizhi{ 0 };
        int huodePLCbaojingxinxidizhi{ 0 };
        int keyipaizhaoxinhaodizhi{ 0 };
        int tingzhipaizhaoxinhaodizhi{ 0 };
        int daizishicechangdudizhi{ 0 };
        int lastChooseLight{ 0 };
    };

    inline SetConfig::SetConfig(const rw::oso::ObjectStoreAssembly& assembly)
    {
        auto isAccountAssembly = assembly.getName();
        if (isAccountAssembly != "$class$SetConfig$")
        {
            throw std::runtime_error("Assembly is not $class$SetConfig$");
        }
        auto xiangjiguangdianpingbishijianItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiangjiguangdianpingbishijian$"));
        if (!xiangjiguangdianpingbishijianItem) {
            throw std::runtime_error("$variable$xiangjiguangdianpingbishijian is not found");
        }
        xiangjiguangdianpingbishijian = xiangjiguangdianpingbishijianItem->getValueAsDouble();
        auto shuchuxinhaoyanshiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shuchuxinhaoyanshi$"));
        if (!shuchuxinhaoyanshiItem) {
            throw std::runtime_error("$variable$shuchuxinhaoyanshi is not found");
        }
        shuchuxinhaoyanshi = shuchuxinhaoyanshiItem->getValueAsDouble();
        auto shuchuxinhaochixushijianItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shuchuxinhaochixushijian$"));
        if (!shuchuxinhaochixushijianItem) {
            throw std::runtime_error("$variable$shuchuxinhaochixushijian is not found");
        }
        shuchuxinhaochixushijian = shuchuxinhaochixushijianItem->getValueAsDouble();
        auto saveImgModeItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$saveImgMode$"));
        if (!saveImgModeItem) {
            throw std::runtime_error("$variable$saveImgMode is not found");
        }
        saveImgMode = saveImgModeItem->getValueAsInt();
        auto scoreItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$score$"));
        if (!scoreItem) {
            throw std::runtime_error("$variable$score is not found");
        }
        score = scoreItem->getValueAsInt();
        auto autoSaveImgItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoSaveImg$"));
        if (!autoSaveImgItem) {
            throw std::runtime_error("$variable$autoSaveImg is not found");
        }
        autoSaveImg = autoSaveImgItem->getValueAsBool();
        auto shangxianwei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shangxianwei1$"));
        if (!shangxianwei1Item) {
            throw std::runtime_error("$variable$shangxianwei1 is not found");
        }
        shangxianwei1 = shangxianwei1Item->getValueAsDouble();
        auto xiaxianwei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiaxianwei1$"));
        if (!xiaxianwei1Item) {
            throw std::runtime_error("$variable$xiaxianwei1 is not found");
        }
        xiaxianwei1 = xiaxianwei1Item->getValueAsDouble();
        auto zuoxianwei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zuoxianwei1$"));
        if (!zuoxianwei1Item) {
            throw std::runtime_error("$variable$zuoxianwei1 is not found");
        }
        zuoxianwei1 = zuoxianwei1Item->getValueAsDouble();
        auto youxianwei1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$youxianwei1$"));
        if (!youxianwei1Item) {
            throw std::runtime_error("$variable$youxianwei1 is not found");
        }
        youxianwei1 = youxianwei1Item->getValueAsDouble();
        auto xiangsudangliang1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiangsudangliang1$"));
        if (!xiangsudangliang1Item) {
            throw std::runtime_error("$variable$xiangsudangliang1 is not found");
        }
        xiangsudangliang1 = xiangsudangliang1Item->getValueAsDouble();
        auto debugFrameItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$debugFrame$"));
        if (!debugFrameItem) {
            throw std::runtime_error("$variable$debugFrame is not found");
        }
        debugFrame = debugFrameItem->getValueAsInt();
        auto openRemoveFrameItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$openRemoveFrame$"));
        if (!openRemoveFrameItem) {
            throw std::runtime_error("$variable$openRemoveFrame is not found");
        }
        openRemoveFrame = openRemoveFrameItem->getValueAsInt();
        auto zengyiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zengyi$"));
        if (!zengyiItem) {
            throw std::runtime_error("$variable$zengyi is not found");
        }
        zengyi = zengyiItem->getValueAsDouble();
        auto qiangguangItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$qiangguang$"));
        if (!qiangguangItem) {
            throw std::runtime_error("$variable$qiangguang is not found");
        }
        qiangguang = qiangguangItem->getValueAsDouble();
        auto zhongguangItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zhongguang$"));
        if (!zhongguangItem) {
            throw std::runtime_error("$variable$zhongguang is not found");
        }
        zhongguang = zhongguangItem->getValueAsDouble();
        auto ruoguangItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$ruoguang$"));
        if (!ruoguangItem) {
            throw std::runtime_error("$variable$ruoguang is not found");
        }
        ruoguang = ruoguangItem->getValueAsDouble();
        auto shiceyahenkuanduduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shiceyahenkuanduduqudizhi$"));
        if (!shiceyahenkuanduduqudizhiItem) {
            throw std::runtime_error("$variable$shiceyahenkuanduduqudizhi is not found");
        }
        shiceyahenkuanduduqudizhi = shiceyahenkuanduduqudizhiItem->getValueAsInt();
        auto shedingyahenbiaozhunzhiduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shedingyahenbiaozhunzhiduqudizhi$"));
        if (!shedingyahenbiaozhunzhiduqudizhiItem) {
            throw std::runtime_error("$variable$shedingyahenbiaozhunzhiduqudizhi is not found");
        }
        shedingyahenbiaozhunzhiduqudizhi = shedingyahenbiaozhunzhiduqudizhiItem->getValueAsInt();
        auto paizhaojiangejuliduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$paizhaojiangejuliduqudizhi$"));
        if (!paizhaojiangejuliduqudizhiItem) {
            throw std::runtime_error("$variable$paizhaojiangejuliduqudizhi is not found");
        }
        paizhaojiangejuliduqudizhi = paizhaojiangejuliduqudizhiItem->getValueAsInt();
        auto daizichangdubiaozhunduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$daizichangdubiaozhunduqudizhi$"));
        if (!daizichangdubiaozhunduqudizhiItem) {
            throw std::runtime_error("$variable$daizichangdubiaozhunduqudizhi is not found");
        }
        daizichangdubiaozhunduqudizhi = daizichangdubiaozhunduqudizhiItem->getValueAsInt();
        auto jimiqiyiquanmaichongshuduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$jimiqiyiquanmaichongshuduqudizhi$"));
        if (!jimiqiyiquanmaichongshuduqudizhiItem) {
            throw std::runtime_error("$variable$jimiqiyiquanmaichongshuduqudizhi is not found");
        }
        jimiqiyiquanmaichongshuduqudizhi = jimiqiyiquanmaichongshuduqudizhiItem->getValueAsInt();
        auto jimiqiyiquanzhouchangduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$jimiqiyiquanzhouchangduqudizhi$"));
        if (!jimiqiyiquanzhouchangduqudizhiItem) {
            throw std::runtime_error("$variable$jimiqiyiquanzhouchangduqudizhi is not found");
        }
        jimiqiyiquanzhouchangduqudizhi = jimiqiyiquanzhouchangduqudizhiItem->getValueAsInt();
        auto bujinyiquanmaichongshuduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$bujinyiquanmaichongshuduqudizhi$"));
        if (!bujinyiquanmaichongshuduqudizhiItem) {
            throw std::runtime_error("$variable$bujinyiquanmaichongshuduqudizhi is not found");
        }
        bujinyiquanmaichongshuduqudizhi = bujinyiquanmaichongshuduqudizhiItem->getValueAsInt();
        auto bujinluojuduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$bujinluojuduqudizhi$"));
        if (!bujinluojuduqudizhiItem) {
            throw std::runtime_error("$variable$bujinluojuduqudizhi is not found");
        }
        bujinluojuduqudizhi = bujinluojuduqudizhiItem->getValueAsInt();
        auto daoyidongbuchangduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$daoyidongbuchangduqudizhi$"));
        if (!daoyidongbuchangduqudizhiItem) {
            throw std::runtime_error("$variable$daoyidongbuchangduqudizhi is not found");
        }
        daoyidongbuchangduqudizhi = daoyidongbuchangduqudizhiItem->getValueAsInt();
        auto zidongdaoyidongzuidajuliduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zidongdaoyidongzuidajuliduqudizhi$"));
        if (!zidongdaoyidongzuidajuliduqudizhiItem) {
            throw std::runtime_error("$variable$zidongdaoyidongzuidajuliduqudizhi is not found");
        }
        zidongdaoyidongzuidajuliduqudizhi = zidongdaoyidongzuidajuliduqudizhiItem->getValueAsInt();
        auto daojiakeyidongdezuidajuliduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$daojiakeyidongdezuidajuliduqudizhi$"));
        if (!daojiakeyidongdezuidajuliduqudizhiItem) {
            throw std::runtime_error("$variable$daojiakeyidongdezuidajuliduqudizhi is not found");
        }
        daojiakeyidongdezuidajuliduqudizhi = daojiakeyidongdezuidajuliduqudizhiItem->getValueAsInt();
        auto daoyidongsudumaichongpinlvduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$daoyidongsudumaichongpinlvduqudizhi$"));
        if (!daoyidongsudumaichongpinlvduqudizhiItem) {
            throw std::runtime_error("$variable$daoyidongsudumaichongpinlvduqudizhi is not found");
        }
        daoyidongsudumaichongpinlvduqudizhi = daoyidongsudumaichongpinlvduqudizhiItem->getValueAsInt();
        auto huodePLCbaojingxinxidizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$huodePLCbaojingxinxidizhi$"));
        if (!huodePLCbaojingxinxidizhiItem) {
            throw std::runtime_error("$variable$huodePLCbaojingxinxidizhi is not found");
        }
        huodePLCbaojingxinxidizhi = huodePLCbaojingxinxidizhiItem->getValueAsInt();
        auto keyipaizhaoxinhaodizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$keyipaizhaoxinhaodizhi$"));
        if (!keyipaizhaoxinhaodizhiItem) {
            throw std::runtime_error("$variable$keyipaizhaoxinhaodizhi is not found");
        }
        keyipaizhaoxinhaodizhi = keyipaizhaoxinhaodizhiItem->getValueAsInt();
        auto tingzhipaizhaoxinhaodizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$tingzhipaizhaoxinhaodizhi$"));
        if (!tingzhipaizhaoxinhaodizhiItem) {
            throw std::runtime_error("$variable$tingzhipaizhaoxinhaodizhi is not found");
        }
        tingzhipaizhaoxinhaodizhi = tingzhipaizhaoxinhaodizhiItem->getValueAsInt();
        auto daizishicechangdudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$daizishicechangdudizhi$"));
        if (!daizishicechangdudizhiItem) {
            throw std::runtime_error("$variable$daizishicechangdudizhi is not found");
        }
        daizishicechangdudizhi = daizishicechangdudizhiItem->getValueAsInt();
        auto lastChooseLightItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$lastChooseLight$"));
        if (!lastChooseLightItem) {
            throw std::runtime_error("$variable$lastChooseLight is not found");
        }
        lastChooseLight = lastChooseLightItem->getValueAsInt();
    }

    inline SetConfig::SetConfig(const SetConfig& obj)
    {
        xiangjiguangdianpingbishijian = obj.xiangjiguangdianpingbishijian;
        shuchuxinhaoyanshi = obj.shuchuxinhaoyanshi;
        shuchuxinhaochixushijian = obj.shuchuxinhaochixushijian;
        saveImgMode = obj.saveImgMode;
        score = obj.score;
        autoSaveImg = obj.autoSaveImg;
        shangxianwei1 = obj.shangxianwei1;
        xiaxianwei1 = obj.xiaxianwei1;
        zuoxianwei1 = obj.zuoxianwei1;
        youxianwei1 = obj.youxianwei1;
        xiangsudangliang1 = obj.xiangsudangliang1;
        debugFrame = obj.debugFrame;
        openRemoveFrame = obj.openRemoveFrame;
        zengyi = obj.zengyi;
        qiangguang = obj.qiangguang;
        zhongguang = obj.zhongguang;
        ruoguang = obj.ruoguang;
        shiceyahenkuanduduqudizhi = obj.shiceyahenkuanduduqudizhi;
        shedingyahenbiaozhunzhiduqudizhi = obj.shedingyahenbiaozhunzhiduqudizhi;
        paizhaojiangejuliduqudizhi = obj.paizhaojiangejuliduqudizhi;
        daizichangdubiaozhunduqudizhi = obj.daizichangdubiaozhunduqudizhi;
        jimiqiyiquanmaichongshuduqudizhi = obj.jimiqiyiquanmaichongshuduqudizhi;
        jimiqiyiquanzhouchangduqudizhi = obj.jimiqiyiquanzhouchangduqudizhi;
        bujinyiquanmaichongshuduqudizhi = obj.bujinyiquanmaichongshuduqudizhi;
        bujinluojuduqudizhi = obj.bujinluojuduqudizhi;
        daoyidongbuchangduqudizhi = obj.daoyidongbuchangduqudizhi;
        zidongdaoyidongzuidajuliduqudizhi = obj.zidongdaoyidongzuidajuliduqudizhi;
        daojiakeyidongdezuidajuliduqudizhi = obj.daojiakeyidongdezuidajuliduqudizhi;
        daoyidongsudumaichongpinlvduqudizhi = obj.daoyidongsudumaichongpinlvduqudizhi;
        huodePLCbaojingxinxidizhi = obj.huodePLCbaojingxinxidizhi;
        keyipaizhaoxinhaodizhi = obj.keyipaizhaoxinhaodizhi;
        tingzhipaizhaoxinhaodizhi = obj.tingzhipaizhaoxinhaodizhi;
        daizishicechangdudizhi = obj.daizishicechangdudizhi;
        lastChooseLight = obj.lastChooseLight;
    }

    inline SetConfig& SetConfig::operator=(const SetConfig& obj)
    {
        if (this != &obj) {
            xiangjiguangdianpingbishijian = obj.xiangjiguangdianpingbishijian;
            shuchuxinhaoyanshi = obj.shuchuxinhaoyanshi;
            shuchuxinhaochixushijian = obj.shuchuxinhaochixushijian;
            saveImgMode = obj.saveImgMode;
            score = obj.score;
            autoSaveImg = obj.autoSaveImg;
            shangxianwei1 = obj.shangxianwei1;
            xiaxianwei1 = obj.xiaxianwei1;
            zuoxianwei1 = obj.zuoxianwei1;
            youxianwei1 = obj.youxianwei1;
            xiangsudangliang1 = obj.xiangsudangliang1;
            debugFrame = obj.debugFrame;
            openRemoveFrame = obj.openRemoveFrame;
            zengyi = obj.zengyi;
            qiangguang = obj.qiangguang;
            zhongguang = obj.zhongguang;
            ruoguang = obj.ruoguang;
            shiceyahenkuanduduqudizhi = obj.shiceyahenkuanduduqudizhi;
            shedingyahenbiaozhunzhiduqudizhi = obj.shedingyahenbiaozhunzhiduqudizhi;
            paizhaojiangejuliduqudizhi = obj.paizhaojiangejuliduqudizhi;
            daizichangdubiaozhunduqudizhi = obj.daizichangdubiaozhunduqudizhi;
            jimiqiyiquanmaichongshuduqudizhi = obj.jimiqiyiquanmaichongshuduqudizhi;
            jimiqiyiquanzhouchangduqudizhi = obj.jimiqiyiquanzhouchangduqudizhi;
            bujinyiquanmaichongshuduqudizhi = obj.bujinyiquanmaichongshuduqudizhi;
            bujinluojuduqudizhi = obj.bujinluojuduqudizhi;
            daoyidongbuchangduqudizhi = obj.daoyidongbuchangduqudizhi;
            zidongdaoyidongzuidajuliduqudizhi = obj.zidongdaoyidongzuidajuliduqudizhi;
            daojiakeyidongdezuidajuliduqudizhi = obj.daojiakeyidongdezuidajuliduqudizhi;
            daoyidongsudumaichongpinlvduqudizhi = obj.daoyidongsudumaichongpinlvduqudizhi;
            huodePLCbaojingxinxidizhi = obj.huodePLCbaojingxinxidizhi;
            keyipaizhaoxinhaodizhi = obj.keyipaizhaoxinhaodizhi;
            tingzhipaizhaoxinhaodizhi = obj.tingzhipaizhaoxinhaodizhi;
            daizishicechangdudizhi = obj.daizishicechangdudizhi;
            lastChooseLight = obj.lastChooseLight;
        }
        return *this;
    }

    inline SetConfig::operator rw::oso::ObjectStoreAssembly() const
    {
        rw::oso::ObjectStoreAssembly assembly;
        assembly.setName("$class$SetConfig$");
        auto xiangjiguangdianpingbishijianItem = std::make_shared<rw::oso::ObjectStoreItem>();
        xiangjiguangdianpingbishijianItem->setName("$variable$xiangjiguangdianpingbishijian$");
        xiangjiguangdianpingbishijianItem->setValueFromDouble(xiangjiguangdianpingbishijian);
        assembly.addItem(xiangjiguangdianpingbishijianItem);
        auto shuchuxinhaoyanshiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        shuchuxinhaoyanshiItem->setName("$variable$shuchuxinhaoyanshi$");
        shuchuxinhaoyanshiItem->setValueFromDouble(shuchuxinhaoyanshi);
        assembly.addItem(shuchuxinhaoyanshiItem);
        auto shuchuxinhaochixushijianItem = std::make_shared<rw::oso::ObjectStoreItem>();
        shuchuxinhaochixushijianItem->setName("$variable$shuchuxinhaochixushijian$");
        shuchuxinhaochixushijianItem->setValueFromDouble(shuchuxinhaochixushijian);
        assembly.addItem(shuchuxinhaochixushijianItem);
        auto saveImgModeItem = std::make_shared<rw::oso::ObjectStoreItem>();
        saveImgModeItem->setName("$variable$saveImgMode$");
        saveImgModeItem->setValueFromInt(saveImgMode);
        assembly.addItem(saveImgModeItem);
        auto scoreItem = std::make_shared<rw::oso::ObjectStoreItem>();
        scoreItem->setName("$variable$score$");
        scoreItem->setValueFromInt(score);
        assembly.addItem(scoreItem);
        auto autoSaveImgItem = std::make_shared<rw::oso::ObjectStoreItem>();
        autoSaveImgItem->setName("$variable$autoSaveImg$");
        autoSaveImgItem->setValueFromBool(autoSaveImg);
        assembly.addItem(autoSaveImgItem);
        auto shangxianwei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shangxianwei1Item->setName("$variable$shangxianwei1$");
        shangxianwei1Item->setValueFromDouble(shangxianwei1);
        assembly.addItem(shangxianwei1Item);
        auto xiaxianwei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiaxianwei1Item->setName("$variable$xiaxianwei1$");
        xiaxianwei1Item->setValueFromDouble(xiaxianwei1);
        assembly.addItem(xiaxianwei1Item);
        auto zuoxianwei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zuoxianwei1Item->setName("$variable$zuoxianwei1$");
        zuoxianwei1Item->setValueFromDouble(zuoxianwei1);
        assembly.addItem(zuoxianwei1Item);
        auto youxianwei1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        youxianwei1Item->setName("$variable$youxianwei1$");
        youxianwei1Item->setValueFromDouble(youxianwei1);
        assembly.addItem(youxianwei1Item);
        auto xiangsudangliang1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiangsudangliang1Item->setName("$variable$xiangsudangliang1$");
        xiangsudangliang1Item->setValueFromDouble(xiangsudangliang1);
        assembly.addItem(xiangsudangliang1Item);
        auto debugFrameItem = std::make_shared<rw::oso::ObjectStoreItem>();
        debugFrameItem->setName("$variable$debugFrame$");
        debugFrameItem->setValueFromInt(debugFrame);
        assembly.addItem(debugFrameItem);
        auto openRemoveFrameItem = std::make_shared<rw::oso::ObjectStoreItem>();
        openRemoveFrameItem->setName("$variable$openRemoveFrame$");
        openRemoveFrameItem->setValueFromInt(openRemoveFrame);
        assembly.addItem(openRemoveFrameItem);
        auto zengyiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        zengyiItem->setName("$variable$zengyi$");
        zengyiItem->setValueFromDouble(zengyi);
        assembly.addItem(zengyiItem);
        auto qiangguangItem = std::make_shared<rw::oso::ObjectStoreItem>();
        qiangguangItem->setName("$variable$qiangguang$");
        qiangguangItem->setValueFromDouble(qiangguang);
        assembly.addItem(qiangguangItem);
        auto zhongguangItem = std::make_shared<rw::oso::ObjectStoreItem>();
        zhongguangItem->setName("$variable$zhongguang$");
        zhongguangItem->setValueFromDouble(zhongguang);
        assembly.addItem(zhongguangItem);
        auto ruoguangItem = std::make_shared<rw::oso::ObjectStoreItem>();
        ruoguangItem->setName("$variable$ruoguang$");
        ruoguangItem->setValueFromDouble(ruoguang);
        assembly.addItem(ruoguangItem);
        auto shiceyahenkuanduduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        shiceyahenkuanduduqudizhiItem->setName("$variable$shiceyahenkuanduduqudizhi$");
        shiceyahenkuanduduqudizhiItem->setValueFromInt(shiceyahenkuanduduqudizhi);
        assembly.addItem(shiceyahenkuanduduqudizhiItem);
        auto shedingyahenbiaozhunzhiduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        shedingyahenbiaozhunzhiduqudizhiItem->setName("$variable$shedingyahenbiaozhunzhiduqudizhi$");
        shedingyahenbiaozhunzhiduqudizhiItem->setValueFromInt(shedingyahenbiaozhunzhiduqudizhi);
        assembly.addItem(shedingyahenbiaozhunzhiduqudizhiItem);
        auto paizhaojiangejuliduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        paizhaojiangejuliduqudizhiItem->setName("$variable$paizhaojiangejuliduqudizhi$");
        paizhaojiangejuliduqudizhiItem->setValueFromInt(paizhaojiangejuliduqudizhi);
        assembly.addItem(paizhaojiangejuliduqudizhiItem);
        auto daizichangdubiaozhunduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        daizichangdubiaozhunduqudizhiItem->setName("$variable$daizichangdubiaozhunduqudizhi$");
        daizichangdubiaozhunduqudizhiItem->setValueFromInt(daizichangdubiaozhunduqudizhi);
        assembly.addItem(daizichangdubiaozhunduqudizhiItem);
        auto jimiqiyiquanmaichongshuduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        jimiqiyiquanmaichongshuduqudizhiItem->setName("$variable$jimiqiyiquanmaichongshuduqudizhi$");
        jimiqiyiquanmaichongshuduqudizhiItem->setValueFromInt(jimiqiyiquanmaichongshuduqudizhi);
        assembly.addItem(jimiqiyiquanmaichongshuduqudizhiItem);
        auto jimiqiyiquanzhouchangduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        jimiqiyiquanzhouchangduqudizhiItem->setName("$variable$jimiqiyiquanzhouchangduqudizhi$");
        jimiqiyiquanzhouchangduqudizhiItem->setValueFromInt(jimiqiyiquanzhouchangduqudizhi);
        assembly.addItem(jimiqiyiquanzhouchangduqudizhiItem);
        auto bujinyiquanmaichongshuduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        bujinyiquanmaichongshuduqudizhiItem->setName("$variable$bujinyiquanmaichongshuduqudizhi$");
        bujinyiquanmaichongshuduqudizhiItem->setValueFromInt(bujinyiquanmaichongshuduqudizhi);
        assembly.addItem(bujinyiquanmaichongshuduqudizhiItem);
        auto bujinluojuduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        bujinluojuduqudizhiItem->setName("$variable$bujinluojuduqudizhi$");
        bujinluojuduqudizhiItem->setValueFromInt(bujinluojuduqudizhi);
        assembly.addItem(bujinluojuduqudizhiItem);
        auto daoyidongbuchangduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        daoyidongbuchangduqudizhiItem->setName("$variable$daoyidongbuchangduqudizhi$");
        daoyidongbuchangduqudizhiItem->setValueFromInt(daoyidongbuchangduqudizhi);
        assembly.addItem(daoyidongbuchangduqudizhiItem);
        auto zidongdaoyidongzuidajuliduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        zidongdaoyidongzuidajuliduqudizhiItem->setName("$variable$zidongdaoyidongzuidajuliduqudizhi$");
        zidongdaoyidongzuidajuliduqudizhiItem->setValueFromInt(zidongdaoyidongzuidajuliduqudizhi);
        assembly.addItem(zidongdaoyidongzuidajuliduqudizhiItem);
        auto daojiakeyidongdezuidajuliduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        daojiakeyidongdezuidajuliduqudizhiItem->setName("$variable$daojiakeyidongdezuidajuliduqudizhi$");
        daojiakeyidongdezuidajuliduqudizhiItem->setValueFromInt(daojiakeyidongdezuidajuliduqudizhi);
        assembly.addItem(daojiakeyidongdezuidajuliduqudizhiItem);
        auto daoyidongsudumaichongpinlvduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        daoyidongsudumaichongpinlvduqudizhiItem->setName("$variable$daoyidongsudumaichongpinlvduqudizhi$");
        daoyidongsudumaichongpinlvduqudizhiItem->setValueFromInt(daoyidongsudumaichongpinlvduqudizhi);
        assembly.addItem(daoyidongsudumaichongpinlvduqudizhiItem);
        auto huodePLCbaojingxinxidizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        huodePLCbaojingxinxidizhiItem->setName("$variable$huodePLCbaojingxinxidizhi$");
        huodePLCbaojingxinxidizhiItem->setValueFromInt(huodePLCbaojingxinxidizhi);
        assembly.addItem(huodePLCbaojingxinxidizhiItem);
        auto keyipaizhaoxinhaodizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        keyipaizhaoxinhaodizhiItem->setName("$variable$keyipaizhaoxinhaodizhi$");
        keyipaizhaoxinhaodizhiItem->setValueFromInt(keyipaizhaoxinhaodizhi);
        assembly.addItem(keyipaizhaoxinhaodizhiItem);
        auto tingzhipaizhaoxinhaodizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        tingzhipaizhaoxinhaodizhiItem->setName("$variable$tingzhipaizhaoxinhaodizhi$");
        tingzhipaizhaoxinhaodizhiItem->setValueFromInt(tingzhipaizhaoxinhaodizhi);
        assembly.addItem(tingzhipaizhaoxinhaodizhiItem);
        auto daizishicechangdudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        daizishicechangdudizhiItem->setName("$variable$daizishicechangdudizhi$");
        daizishicechangdudizhiItem->setValueFromInt(daizishicechangdudizhi);
        assembly.addItem(daizishicechangdudizhiItem);
        auto lastChooseLightItem = std::make_shared<rw::oso::ObjectStoreItem>();
        lastChooseLightItem->setName("$variable$lastChooseLight$");
        lastChooseLightItem->setValueFromInt(lastChooseLight);
        assembly.addItem(lastChooseLightItem);
        return assembly;
    }

    inline bool SetConfig::operator==(const SetConfig& obj) const
    {
        return xiangjiguangdianpingbishijian == obj.xiangjiguangdianpingbishijian && shuchuxinhaoyanshi == obj.shuchuxinhaoyanshi && shuchuxinhaochixushijian == obj.shuchuxinhaochixushijian && saveImgMode == obj.saveImgMode && score == obj.score && autoSaveImg == obj.autoSaveImg && shangxianwei1 == obj.shangxianwei1 && xiaxianwei1 == obj.xiaxianwei1 && zuoxianwei1 == obj.zuoxianwei1 && youxianwei1 == obj.youxianwei1 && xiangsudangliang1 == obj.xiangsudangliang1 && debugFrame == obj.debugFrame && openRemoveFrame == obj.openRemoveFrame && zengyi == obj.zengyi && qiangguang == obj.qiangguang && zhongguang == obj.zhongguang && ruoguang == obj.ruoguang && shiceyahenkuanduduqudizhi == obj.shiceyahenkuanduduqudizhi && shedingyahenbiaozhunzhiduqudizhi == obj.shedingyahenbiaozhunzhiduqudizhi && paizhaojiangejuliduqudizhi == obj.paizhaojiangejuliduqudizhi && daizichangdubiaozhunduqudizhi == obj.daizichangdubiaozhunduqudizhi && jimiqiyiquanmaichongshuduqudizhi == obj.jimiqiyiquanmaichongshuduqudizhi && jimiqiyiquanzhouchangduqudizhi == obj.jimiqiyiquanzhouchangduqudizhi && bujinyiquanmaichongshuduqudizhi == obj.bujinyiquanmaichongshuduqudizhi && bujinluojuduqudizhi == obj.bujinluojuduqudizhi && daoyidongbuchangduqudizhi == obj.daoyidongbuchangduqudizhi && zidongdaoyidongzuidajuliduqudizhi == obj.zidongdaoyidongzuidajuliduqudizhi && daojiakeyidongdezuidajuliduqudizhi == obj.daojiakeyidongdezuidajuliduqudizhi && daoyidongsudumaichongpinlvduqudizhi == obj.daoyidongsudumaichongpinlvduqudizhi && huodePLCbaojingxinxidizhi == obj.huodePLCbaojingxinxidizhi && keyipaizhaoxinhaodizhi == obj.keyipaizhaoxinhaodizhi && tingzhipaizhaoxinhaodizhi == obj.tingzhipaizhaoxinhaodizhi && daizishicechangdudizhi == obj.daizishicechangdudizhi && lastChooseLight == obj.lastChooseLight;
    }

    inline bool SetConfig::operator!=(const SetConfig& obj) const
    {
        return !(*this == obj);
    }

}

