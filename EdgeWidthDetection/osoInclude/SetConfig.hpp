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
        int debugFrame1{ 0 };
        int openRemoveFrame1{ 0 };
        double zengyi1{ 0 };
        double qiangguang1{ 0 };
        double zhongguang1{ 0 };
        double ruoguang1{ 0 };
        double shangxianwei2{ 0 };
        double xiaxianwei2{ 0 };
        double zuoxianwei2{ 0 };
        double youxianwei2{ 0 };
        double xiangsudangliang2{ 0 };
        int debugFrame2{ 0 };
        int openRemoveFrame2{ 0 };
        double zengyi2{ 0 };
        double qiangguang2{ 0 };
        double zhongguang2{ 0 };
        double ruoguang2{ 0 };
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
        int shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi{ 0 };
        bool shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1{ false };
        bool shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2{ false };
        int huodePLCbaojingxinxidizhi{ 0 };
        int keyipaizhaoxinhaodizhi{ 0 };
        int tingzhipaizhaoxinhaodizhi{ 0 };
        int daizishicechangdudizhi{ 0 };
        int shiceyahenkuanduxieruqishidizhi1{ 0 };
        int shiceyahenkuanduxierumoweidizhi1{ 0 };
        int shiceyahenkuandushanchujiushujujiange1{ 0 };
        int tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1{ 0 };
        int tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1{ 0 };
        int tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1{ 0 };
        int shiceyahenkuanduxieruqishidizhi2{ 0 };
        int shiceyahenkuanduxierumoweidizhi2{ 0 };
        int shiceyahenkuandushanchujiushujujiange2{ 0 };
        int lastChooseLight1{ 0 };
        int lastChooseLight2{ 0 };
        bool autoExposureEnabled1{ false };
        bool autoExposureEnabled2{ false };
        double autoExposureTargetMean{ 128.0 };
        double autoExposureOverExposeThreshold{ 250.0 };
        double autoExposureUnderExposeThreshold{ 20.0 };
        double autoExposureMaxOverRatio{ 0.02 };
        double autoExposureMaxUnderRatio{ 0.05 };
        double autoExposureMinExposure{ 100.0 };
        double autoExposureMaxExposure{ 50000.0 };
        double autoExposureAdjustIntervalMs{ 500.0 };
        double autoExposureLastExposure1{ 0.0 };
        double autoExposureLastExposure2{ 0.0 };
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
        auto debugFrame1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$debugFrame1$"));
        if (!debugFrame1Item) {
            throw std::runtime_error("$variable$debugFrame1 is not found");
        }
        debugFrame1 = debugFrame1Item->getValueAsInt();
        auto openRemoveFrame1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$openRemoveFrame1$"));
        if (!openRemoveFrame1Item) {
            throw std::runtime_error("$variable$openRemoveFrame1 is not found");
        }
        openRemoveFrame1 = openRemoveFrame1Item->getValueAsInt();
        auto zengyi1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zengyi1$"));
        if (!zengyi1Item) {
            throw std::runtime_error("$variable$zengyi1 is not found");
        }
        zengyi1 = zengyi1Item->getValueAsDouble();
        auto qiangguang1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$qiangguang1$"));
        if (!qiangguang1Item) {
            throw std::runtime_error("$variable$qiangguang1 is not found");
        }
        qiangguang1 = qiangguang1Item->getValueAsDouble();
        auto zhongguang1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zhongguang1$"));
        if (!zhongguang1Item) {
            throw std::runtime_error("$variable$zhongguang1 is not found");
        }
        zhongguang1 = zhongguang1Item->getValueAsDouble();
        auto ruoguang1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$ruoguang1$"));
        if (!ruoguang1Item) {
            throw std::runtime_error("$variable$ruoguang1 is not found");
        }
        ruoguang1 = ruoguang1Item->getValueAsDouble();
        auto shangxianwei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shangxianwei2$"));
        if (!shangxianwei2Item) {
            throw std::runtime_error("$variable$shangxianwei2 is not found");
        }
        shangxianwei2 = shangxianwei2Item->getValueAsDouble();
        auto xiaxianwei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiaxianwei2$"));
        if (!xiaxianwei2Item) {
            throw std::runtime_error("$variable$xiaxianwei2 is not found");
        }
        xiaxianwei2 = xiaxianwei2Item->getValueAsDouble();
        auto zuoxianwei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zuoxianwei2$"));
        if (!zuoxianwei2Item) {
            throw std::runtime_error("$variable$zuoxianwei2 is not found");
        }
        zuoxianwei2 = zuoxianwei2Item->getValueAsDouble();
        auto youxianwei2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$youxianwei2$"));
        if (!youxianwei2Item) {
            throw std::runtime_error("$variable$youxianwei2 is not found");
        }
        youxianwei2 = youxianwei2Item->getValueAsDouble();
        auto xiangsudangliang2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$xiangsudangliang2$"));
        if (!xiangsudangliang2Item) {
            throw std::runtime_error("$variable$xiangsudangliang2 is not found");
        }
        xiangsudangliang2 = xiangsudangliang2Item->getValueAsDouble();
        auto debugFrame2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$debugFrame2$"));
        if (!debugFrame2Item) {
            throw std::runtime_error("$variable$debugFrame2 is not found");
        }
        debugFrame2 = debugFrame2Item->getValueAsInt();
        auto openRemoveFrame2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$openRemoveFrame2$"));
        if (!openRemoveFrame2Item) {
            throw std::runtime_error("$variable$openRemoveFrame2 is not found");
        }
        openRemoveFrame2 = openRemoveFrame2Item->getValueAsInt();
        auto zengyi2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zengyi2$"));
        if (!zengyi2Item) {
            throw std::runtime_error("$variable$zengyi2 is not found");
        }
        zengyi2 = zengyi2Item->getValueAsDouble();
        auto qiangguang2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$qiangguang2$"));
        if (!qiangguang2Item) {
            throw std::runtime_error("$variable$qiangguang2 is not found");
        }
        qiangguang2 = qiangguang2Item->getValueAsDouble();
        auto zhongguang2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$zhongguang2$"));
        if (!zhongguang2Item) {
            throw std::runtime_error("$variable$zhongguang2 is not found");
        }
        zhongguang2 = zhongguang2Item->getValueAsDouble();
        auto ruoguang2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$ruoguang2$"));
        if (!ruoguang2Item) {
            throw std::runtime_error("$variable$ruoguang2 is not found");
        }
        ruoguang2 = ruoguang2Item->getValueAsDouble();
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
        auto shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhiItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi$"));
        if (!shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhiItem) {
            throw std::runtime_error("$variable$shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi is not found");
        }
        shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi = shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhiItem->getValueAsInt();
        auto shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1$"));
        if (!shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1Item) {
            throw std::runtime_error("$variable$shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1 is not found");
        }
        shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1 = shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1Item->getValueAsBool();
        auto shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2$"));
        if (!shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2Item) {
            throw std::runtime_error("$variable$shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2 is not found");
        }
        shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2 = shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2Item->getValueAsBool();
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
        auto shiceyahenkuanduxieruqishidizhi1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shiceyahenkuanduxieruqishidizhi1$"));
        if (!shiceyahenkuanduxieruqishidizhi1Item) {
            throw std::runtime_error("$variable$shiceyahenkuanduxieruqishidizhi1 is not found");
        }
        shiceyahenkuanduxieruqishidizhi1 = shiceyahenkuanduxieruqishidizhi1Item->getValueAsInt();
        auto shiceyahenkuanduxierumoweidizhi1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shiceyahenkuanduxierumoweidizhi1$"));
        if (!shiceyahenkuanduxierumoweidizhi1Item) {
            throw std::runtime_error("$variable$shiceyahenkuanduxierumoweidizhi1 is not found");
        }
        shiceyahenkuanduxierumoweidizhi1 = shiceyahenkuanduxierumoweidizhi1Item->getValueAsInt();
        auto shiceyahenkuandushanchujiushujujiange1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shiceyahenkuandushanchujiushujujiange1$"));
        if (!shiceyahenkuandushanchujiushujujiange1Item) {
            throw std::runtime_error("$variable$shiceyahenkuandushanchujiushujujiange1 is not found");
        }
        shiceyahenkuandushanchujiushujujiange1 = shiceyahenkuandushanchujiushujujiange1Item->getValueAsInt();
        auto tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1$"));
        if (!tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1Item) {
            throw std::runtime_error("$variable$tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1 is not found");
        }
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1 = tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1Item->getValueAsInt();
        auto tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1$"));
        if (!tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1Item) {
            throw std::runtime_error("$variable$tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1 is not found");
        }
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1 = tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1Item->getValueAsInt();
        auto tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1$"));
        if (!tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1Item) {
            throw std::runtime_error("$variable$tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1 is not found");
        }
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1 = tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1Item->getValueAsInt();
        auto shiceyahenkuanduxieruqishidizhi2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shiceyahenkuanduxieruqishidizhi2$"));
        if (!shiceyahenkuanduxieruqishidizhi2Item) {
            throw std::runtime_error("$variable$shiceyahenkuanduxieruqishidizhi2 is not found");
        }
        shiceyahenkuanduxieruqishidizhi2 = shiceyahenkuanduxieruqishidizhi2Item->getValueAsInt();
        auto shiceyahenkuanduxierumoweidizhi2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shiceyahenkuanduxierumoweidizhi2$"));
        if (!shiceyahenkuanduxierumoweidizhi2Item) {
            throw std::runtime_error("$variable$shiceyahenkuanduxierumoweidizhi2 is not found");
        }
        shiceyahenkuanduxierumoweidizhi2 = shiceyahenkuanduxierumoweidizhi2Item->getValueAsInt();
        auto shiceyahenkuandushanchujiushujujiange2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$shiceyahenkuandushanchujiushujujiange2$"));
        if (!shiceyahenkuandushanchujiushujujiange2Item) {
            throw std::runtime_error("$variable$shiceyahenkuandushanchujiushujujiange2 is not found");
        }
        shiceyahenkuandushanchujiushujujiange2 = shiceyahenkuandushanchujiushujujiange2Item->getValueAsInt();
        auto lastChooseLight1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$lastChooseLight1$"));
        if (!lastChooseLight1Item) {
            throw std::runtime_error("$variable$lastChooseLight1 is not found");
        }
        lastChooseLight1 = lastChooseLight1Item->getValueAsInt();
        auto lastChooseLight2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$lastChooseLight2$"));
        if (!lastChooseLight2Item) {
            throw std::runtime_error("$variable$lastChooseLight2 is not found");
        }
        lastChooseLight2 = lastChooseLight2Item->getValueAsInt();
        auto autoExposureEnabled1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureEnabled1$"));
        if (!autoExposureEnabled1Item) {
            throw std::runtime_error("$variable$autoExposureEnabled1 is not found");
        }
        autoExposureEnabled1 = autoExposureEnabled1Item->getValueAsBool();
        auto autoExposureEnabled2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureEnabled2$"));
        if (!autoExposureEnabled2Item) {
            throw std::runtime_error("$variable$autoExposureEnabled2 is not found");
        }
        autoExposureEnabled2 = autoExposureEnabled2Item->getValueAsBool();
        auto autoExposureTargetMeanItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureTargetMean$"));
        if (!autoExposureTargetMeanItem) {
            throw std::runtime_error("$variable$autoExposureTargetMean is not found");
        }
        autoExposureTargetMean = autoExposureTargetMeanItem->getValueAsDouble();
        auto autoExposureOverExposeThresholdItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureOverExposeThreshold$"));
        if (!autoExposureOverExposeThresholdItem) {
            throw std::runtime_error("$variable$autoExposureOverExposeThreshold is not found");
        }
        autoExposureOverExposeThreshold = autoExposureOverExposeThresholdItem->getValueAsDouble();
        auto autoExposureUnderExposeThresholdItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureUnderExposeThreshold$"));
        if (!autoExposureUnderExposeThresholdItem) {
            throw std::runtime_error("$variable$autoExposureUnderExposeThreshold is not found");
        }
        autoExposureUnderExposeThreshold = autoExposureUnderExposeThresholdItem->getValueAsDouble();
        auto autoExposureMaxOverRatioItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureMaxOverRatio$"));
        if (!autoExposureMaxOverRatioItem) {
            throw std::runtime_error("$variable$autoExposureMaxOverRatio is not found");
        }
        autoExposureMaxOverRatio = autoExposureMaxOverRatioItem->getValueAsDouble();
        auto autoExposureMaxUnderRatioItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureMaxUnderRatio$"));
        if (!autoExposureMaxUnderRatioItem) {
            throw std::runtime_error("$variable$autoExposureMaxUnderRatio is not found");
        }
        autoExposureMaxUnderRatio = autoExposureMaxUnderRatioItem->getValueAsDouble();
        auto autoExposureMinExposureItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureMinExposure$"));
        if (!autoExposureMinExposureItem) {
            throw std::runtime_error("$variable$autoExposureMinExposure is not found");
        }
        autoExposureMinExposure = autoExposureMinExposureItem->getValueAsDouble();
        auto autoExposureMaxExposureItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureMaxExposure$"));
        if (!autoExposureMaxExposureItem) {
            throw std::runtime_error("$variable$autoExposureMaxExposure is not found");
        }
        autoExposureMaxExposure = autoExposureMaxExposureItem->getValueAsDouble();
        auto autoExposureAdjustIntervalMsItem = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureAdjustIntervalMs$"));
        if (!autoExposureAdjustIntervalMsItem) {
            throw std::runtime_error("$variable$autoExposureAdjustIntervalMs is not found");
        }
        autoExposureAdjustIntervalMs = autoExposureAdjustIntervalMsItem->getValueAsDouble();
        auto autoExposureLastExposure1Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureLastExposure1$"));
        if (!autoExposureLastExposure1Item) {
            throw std::runtime_error("$variable$autoExposureLastExposure1 is not found");
        }
        autoExposureLastExposure1 = autoExposureLastExposure1Item->getValueAsDouble();
        auto autoExposureLastExposure2Item = rw::oso::ObjectStoreCoreToItem(assembly.getItem("$variable$autoExposureLastExposure2$"));
        if (!autoExposureLastExposure2Item) {
            throw std::runtime_error("$variable$autoExposureLastExposure2 is not found");
        }
        autoExposureLastExposure2 = autoExposureLastExposure2Item->getValueAsDouble();
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
        debugFrame1 = obj.debugFrame1;
        openRemoveFrame1 = obj.openRemoveFrame1;
        zengyi1 = obj.zengyi1;
        qiangguang1 = obj.qiangguang1;
        zhongguang1 = obj.zhongguang1;
        ruoguang1 = obj.ruoguang1;
        shangxianwei2 = obj.shangxianwei2;
        xiaxianwei2 = obj.xiaxianwei2;
        zuoxianwei2 = obj.zuoxianwei2;
        youxianwei2 = obj.youxianwei2;
        xiangsudangliang2 = obj.xiangsudangliang2;
        debugFrame2 = obj.debugFrame2;
        openRemoveFrame2 = obj.openRemoveFrame2;
        zengyi2 = obj.zengyi2;
        qiangguang2 = obj.qiangguang2;
        zhongguang2 = obj.zhongguang2;
        ruoguang2 = obj.ruoguang2;
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
        shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi = obj.shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi;
        shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1 = obj.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1;
        shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2 = obj.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2;
        huodePLCbaojingxinxidizhi = obj.huodePLCbaojingxinxidizhi;
        keyipaizhaoxinhaodizhi = obj.keyipaizhaoxinhaodizhi;
        tingzhipaizhaoxinhaodizhi = obj.tingzhipaizhaoxinhaodizhi;
        daizishicechangdudizhi = obj.daizishicechangdudizhi;
        shiceyahenkuanduxieruqishidizhi1 = obj.shiceyahenkuanduxieruqishidizhi1;
        shiceyahenkuanduxierumoweidizhi1 = obj.shiceyahenkuanduxierumoweidizhi1;
        shiceyahenkuandushanchujiushujujiange1 = obj.shiceyahenkuandushanchujiushujujiange1;
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1 = obj.tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1;
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1 = obj.tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1;
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1 = obj.tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1;
        shiceyahenkuanduxieruqishidizhi2 = obj.shiceyahenkuanduxieruqishidizhi2;
        shiceyahenkuanduxierumoweidizhi2 = obj.shiceyahenkuanduxierumoweidizhi2;
        shiceyahenkuandushanchujiushujujiange2 = obj.shiceyahenkuandushanchujiushujujiange2;
        lastChooseLight1 = obj.lastChooseLight1;
        lastChooseLight2 = obj.lastChooseLight2;
        autoExposureEnabled1 = obj.autoExposureEnabled1;
        autoExposureEnabled2 = obj.autoExposureEnabled2;
        autoExposureTargetMean = obj.autoExposureTargetMean;
        autoExposureOverExposeThreshold = obj.autoExposureOverExposeThreshold;
        autoExposureUnderExposeThreshold = obj.autoExposureUnderExposeThreshold;
        autoExposureMaxOverRatio = obj.autoExposureMaxOverRatio;
        autoExposureMaxUnderRatio = obj.autoExposureMaxUnderRatio;
        autoExposureMinExposure = obj.autoExposureMinExposure;
        autoExposureMaxExposure = obj.autoExposureMaxExposure;
        autoExposureAdjustIntervalMs = obj.autoExposureAdjustIntervalMs;
        autoExposureLastExposure1 = obj.autoExposureLastExposure1;
        autoExposureLastExposure2 = obj.autoExposureLastExposure2;
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
            debugFrame1 = obj.debugFrame1;
            openRemoveFrame1 = obj.openRemoveFrame1;
            zengyi1 = obj.zengyi1;
            qiangguang1 = obj.qiangguang1;
            zhongguang1 = obj.zhongguang1;
            ruoguang1 = obj.ruoguang1;
            shangxianwei2 = obj.shangxianwei2;
            xiaxianwei2 = obj.xiaxianwei2;
            zuoxianwei2 = obj.zuoxianwei2;
            youxianwei2 = obj.youxianwei2;
            xiangsudangliang2 = obj.xiangsudangliang2;
            debugFrame2 = obj.debugFrame2;
            openRemoveFrame2 = obj.openRemoveFrame2;
            zengyi2 = obj.zengyi2;
            qiangguang2 = obj.qiangguang2;
            zhongguang2 = obj.zhongguang2;
            ruoguang2 = obj.ruoguang2;
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
            shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi = obj.shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi;
            shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1 = obj.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1;
            shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2 = obj.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2;
            huodePLCbaojingxinxidizhi = obj.huodePLCbaojingxinxidizhi;
            keyipaizhaoxinhaodizhi = obj.keyipaizhaoxinhaodizhi;
            tingzhipaizhaoxinhaodizhi = obj.tingzhipaizhaoxinhaodizhi;
            daizishicechangdudizhi = obj.daizishicechangdudizhi;
            shiceyahenkuanduxieruqishidizhi1 = obj.shiceyahenkuanduxieruqishidizhi1;
            shiceyahenkuanduxierumoweidizhi1 = obj.shiceyahenkuanduxierumoweidizhi1;
            shiceyahenkuandushanchujiushujujiange1 = obj.shiceyahenkuandushanchujiushujujiange1;
            tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1 = obj.tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1;
            tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1 = obj.tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1;
            tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1 = obj.tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1;
            shiceyahenkuanduxieruqishidizhi2 = obj.shiceyahenkuanduxieruqishidizhi2;
            shiceyahenkuanduxierumoweidizhi2 = obj.shiceyahenkuanduxierumoweidizhi2;
            shiceyahenkuandushanchujiushujujiange2 = obj.shiceyahenkuandushanchujiushujujiange2;
            lastChooseLight1 = obj.lastChooseLight1;
            lastChooseLight2 = obj.lastChooseLight2;
            autoExposureEnabled1 = obj.autoExposureEnabled1;
            autoExposureEnabled2 = obj.autoExposureEnabled2;
            autoExposureTargetMean = obj.autoExposureTargetMean;
            autoExposureOverExposeThreshold = obj.autoExposureOverExposeThreshold;
            autoExposureUnderExposeThreshold = obj.autoExposureUnderExposeThreshold;
            autoExposureMaxOverRatio = obj.autoExposureMaxOverRatio;
            autoExposureMaxUnderRatio = obj.autoExposureMaxUnderRatio;
            autoExposureMinExposure = obj.autoExposureMinExposure;
            autoExposureMaxExposure = obj.autoExposureMaxExposure;
            autoExposureAdjustIntervalMs = obj.autoExposureAdjustIntervalMs;
            autoExposureLastExposure1 = obj.autoExposureLastExposure1;
            autoExposureLastExposure2 = obj.autoExposureLastExposure2;
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
        auto debugFrame1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        debugFrame1Item->setName("$variable$debugFrame1$");
        debugFrame1Item->setValueFromInt(debugFrame1);
        assembly.addItem(debugFrame1Item);
        auto openRemoveFrame1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        openRemoveFrame1Item->setName("$variable$openRemoveFrame1$");
        openRemoveFrame1Item->setValueFromInt(openRemoveFrame1);
        assembly.addItem(openRemoveFrame1Item);
        auto zengyi1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zengyi1Item->setName("$variable$zengyi1$");
        zengyi1Item->setValueFromDouble(zengyi1);
        assembly.addItem(zengyi1Item);
        auto qiangguang1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        qiangguang1Item->setName("$variable$qiangguang1$");
        qiangguang1Item->setValueFromDouble(qiangguang1);
        assembly.addItem(qiangguang1Item);
        auto zhongguang1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zhongguang1Item->setName("$variable$zhongguang1$");
        zhongguang1Item->setValueFromDouble(zhongguang1);
        assembly.addItem(zhongguang1Item);
        auto ruoguang1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        ruoguang1Item->setName("$variable$ruoguang1$");
        ruoguang1Item->setValueFromDouble(ruoguang1);
        assembly.addItem(ruoguang1Item);
        auto shangxianwei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shangxianwei2Item->setName("$variable$shangxianwei2$");
        shangxianwei2Item->setValueFromDouble(shangxianwei2);
        assembly.addItem(shangxianwei2Item);
        auto xiaxianwei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiaxianwei2Item->setName("$variable$xiaxianwei2$");
        xiaxianwei2Item->setValueFromDouble(xiaxianwei2);
        assembly.addItem(xiaxianwei2Item);
        auto zuoxianwei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zuoxianwei2Item->setName("$variable$zuoxianwei2$");
        zuoxianwei2Item->setValueFromDouble(zuoxianwei2);
        assembly.addItem(zuoxianwei2Item);
        auto youxianwei2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        youxianwei2Item->setName("$variable$youxianwei2$");
        youxianwei2Item->setValueFromDouble(youxianwei2);
        assembly.addItem(youxianwei2Item);
        auto xiangsudangliang2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        xiangsudangliang2Item->setName("$variable$xiangsudangliang2$");
        xiangsudangliang2Item->setValueFromDouble(xiangsudangliang2);
        assembly.addItem(xiangsudangliang2Item);
        auto debugFrame2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        debugFrame2Item->setName("$variable$debugFrame2$");
        debugFrame2Item->setValueFromInt(debugFrame2);
        assembly.addItem(debugFrame2Item);
        auto openRemoveFrame2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        openRemoveFrame2Item->setName("$variable$openRemoveFrame2$");
        openRemoveFrame2Item->setValueFromInt(openRemoveFrame2);
        assembly.addItem(openRemoveFrame2Item);
        auto zengyi2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zengyi2Item->setName("$variable$zengyi2$");
        zengyi2Item->setValueFromDouble(zengyi2);
        assembly.addItem(zengyi2Item);
        auto qiangguang2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        qiangguang2Item->setName("$variable$qiangguang2$");
        qiangguang2Item->setValueFromDouble(qiangguang2);
        assembly.addItem(qiangguang2Item);
        auto zhongguang2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        zhongguang2Item->setName("$variable$zhongguang2$");
        zhongguang2Item->setValueFromDouble(zhongguang2);
        assembly.addItem(zhongguang2Item);
        auto ruoguang2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        ruoguang2Item->setName("$variable$ruoguang2$");
        ruoguang2Item->setValueFromDouble(ruoguang2);
        assembly.addItem(ruoguang2Item);
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
        auto shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhiItem = std::make_shared<rw::oso::ObjectStoreItem>();
        shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhiItem->setName("$variable$shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi$");
        shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhiItem->setValueFromInt(shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi);
        assembly.addItem(shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhiItem);
        auto shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1Item->setName("$variable$shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1$");
        shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1Item->setValueFromBool(shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1);
        assembly.addItem(shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1Item);
        auto shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2Item->setName("$variable$shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2$");
        shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2Item->setValueFromBool(shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2);
        assembly.addItem(shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2Item);
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
        auto shiceyahenkuanduxieruqishidizhi1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shiceyahenkuanduxieruqishidizhi1Item->setName("$variable$shiceyahenkuanduxieruqishidizhi1$");
        shiceyahenkuanduxieruqishidizhi1Item->setValueFromInt(shiceyahenkuanduxieruqishidizhi1);
        assembly.addItem(shiceyahenkuanduxieruqishidizhi1Item);
        auto shiceyahenkuanduxierumoweidizhi1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shiceyahenkuanduxierumoweidizhi1Item->setName("$variable$shiceyahenkuanduxierumoweidizhi1$");
        shiceyahenkuanduxierumoweidizhi1Item->setValueFromInt(shiceyahenkuanduxierumoweidizhi1);
        assembly.addItem(shiceyahenkuanduxierumoweidizhi1Item);
        auto shiceyahenkuandushanchujiushujujiange1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shiceyahenkuandushanchujiushujujiange1Item->setName("$variable$shiceyahenkuandushanchujiushujujiange1$");
        shiceyahenkuandushanchujiushujujiange1Item->setValueFromInt(shiceyahenkuandushanchujiushujujiange1);
        assembly.addItem(shiceyahenkuandushanchujiushujujiange1Item);
        auto tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1Item->setName("$variable$tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1$");
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1Item->setValueFromInt(tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1);
        assembly.addItem(tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1Item);
        auto tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1Item->setName("$variable$tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1$");
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1Item->setValueFromInt(tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1);
        assembly.addItem(tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1Item);
        auto tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1Item->setName("$variable$tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1$");
        tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1Item->setValueFromInt(tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1);
        assembly.addItem(tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1Item);
        auto shiceyahenkuanduxieruqishidizhi2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shiceyahenkuanduxieruqishidizhi2Item->setName("$variable$shiceyahenkuanduxieruqishidizhi2$");
        shiceyahenkuanduxieruqishidizhi2Item->setValueFromInt(shiceyahenkuanduxieruqishidizhi2);
        assembly.addItem(shiceyahenkuanduxieruqishidizhi2Item);
        auto shiceyahenkuanduxierumoweidizhi2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shiceyahenkuanduxierumoweidizhi2Item->setName("$variable$shiceyahenkuanduxierumoweidizhi2$");
        shiceyahenkuanduxierumoweidizhi2Item->setValueFromInt(shiceyahenkuanduxierumoweidizhi2);
        assembly.addItem(shiceyahenkuanduxierumoweidizhi2Item);
        auto shiceyahenkuandushanchujiushujujiange2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        shiceyahenkuandushanchujiushujujiange2Item->setName("$variable$shiceyahenkuandushanchujiushujujiange2$");
        shiceyahenkuandushanchujiushujujiange2Item->setValueFromInt(shiceyahenkuandushanchujiushujujiange2);
        assembly.addItem(shiceyahenkuandushanchujiushujujiange2Item);
        auto lastChooseLight1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        lastChooseLight1Item->setName("$variable$lastChooseLight1$");
        lastChooseLight1Item->setValueFromInt(lastChooseLight1);
        assembly.addItem(lastChooseLight1Item);
        auto lastChooseLight2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        lastChooseLight2Item->setName("$variable$lastChooseLight2$");
        lastChooseLight2Item->setValueFromInt(lastChooseLight2);
        assembly.addItem(lastChooseLight2Item);
        auto autoExposureEnabled1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureEnabled1Item->setName("$variable$autoExposureEnabled1$");
        autoExposureEnabled1Item->setValueFromBool(autoExposureEnabled1);
        assembly.addItem(autoExposureEnabled1Item);
        auto autoExposureEnabled2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureEnabled2Item->setName("$variable$autoExposureEnabled2$");
        autoExposureEnabled2Item->setValueFromBool(autoExposureEnabled2);
        assembly.addItem(autoExposureEnabled2Item);
        auto autoExposureTargetMeanItem = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureTargetMeanItem->setName("$variable$autoExposureTargetMean$");
        autoExposureTargetMeanItem->setValueFromDouble(autoExposureTargetMean);
        assembly.addItem(autoExposureTargetMeanItem);
        auto autoExposureOverExposeThresholdItem = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureOverExposeThresholdItem->setName("$variable$autoExposureOverExposeThreshold$");
        autoExposureOverExposeThresholdItem->setValueFromDouble(autoExposureOverExposeThreshold);
        assembly.addItem(autoExposureOverExposeThresholdItem);
        auto autoExposureUnderExposeThresholdItem = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureUnderExposeThresholdItem->setName("$variable$autoExposureUnderExposeThreshold$");
        autoExposureUnderExposeThresholdItem->setValueFromDouble(autoExposureUnderExposeThreshold);
        assembly.addItem(autoExposureUnderExposeThresholdItem);
        auto autoExposureMaxOverRatioItem = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureMaxOverRatioItem->setName("$variable$autoExposureMaxOverRatio$");
        autoExposureMaxOverRatioItem->setValueFromDouble(autoExposureMaxOverRatio);
        assembly.addItem(autoExposureMaxOverRatioItem);
        auto autoExposureMaxUnderRatioItem = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureMaxUnderRatioItem->setName("$variable$autoExposureMaxUnderRatio$");
        autoExposureMaxUnderRatioItem->setValueFromDouble(autoExposureMaxUnderRatio);
        assembly.addItem(autoExposureMaxUnderRatioItem);
        auto autoExposureMinExposureItem = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureMinExposureItem->setName("$variable$autoExposureMinExposure$");
        autoExposureMinExposureItem->setValueFromDouble(autoExposureMinExposure);
        assembly.addItem(autoExposureMinExposureItem);
        auto autoExposureMaxExposureItem = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureMaxExposureItem->setName("$variable$autoExposureMaxExposure$");
        autoExposureMaxExposureItem->setValueFromDouble(autoExposureMaxExposure);
        assembly.addItem(autoExposureMaxExposureItem);
        auto autoExposureAdjustIntervalMsItem = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureAdjustIntervalMsItem->setName("$variable$autoExposureAdjustIntervalMs$");
        autoExposureAdjustIntervalMsItem->setValueFromDouble(autoExposureAdjustIntervalMs);
        assembly.addItem(autoExposureAdjustIntervalMsItem);
        auto autoExposureLastExposure1Item = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureLastExposure1Item->setName("$variable$autoExposureLastExposure1$");
        autoExposureLastExposure1Item->setValueFromDouble(autoExposureLastExposure1);
        assembly.addItem(autoExposureLastExposure1Item);
        auto autoExposureLastExposure2Item = std::make_shared<rw::oso::ObjectStoreItem>();
        autoExposureLastExposure2Item->setName("$variable$autoExposureLastExposure2$");
        autoExposureLastExposure2Item->setValueFromDouble(autoExposureLastExposure2);
        assembly.addItem(autoExposureLastExposure2Item);
        return assembly;
    }

    inline bool SetConfig::operator==(const SetConfig& obj) const
    {
        return xiangjiguangdianpingbishijian == obj.xiangjiguangdianpingbishijian && shuchuxinhaoyanshi == obj.shuchuxinhaoyanshi && shuchuxinhaochixushijian == obj.shuchuxinhaochixushijian && saveImgMode == obj.saveImgMode && score == obj.score && autoSaveImg == obj.autoSaveImg && shangxianwei1 == obj.shangxianwei1 && xiaxianwei1 == obj.xiaxianwei1 && zuoxianwei1 == obj.zuoxianwei1 && youxianwei1 == obj.youxianwei1 && xiangsudangliang1 == obj.xiangsudangliang1 && debugFrame1 == obj.debugFrame1 && openRemoveFrame1 == obj.openRemoveFrame1 && zengyi1 == obj.zengyi1 && qiangguang1 == obj.qiangguang1 && zhongguang1 == obj.zhongguang1 && ruoguang1 == obj.ruoguang1 && shangxianwei2 == obj.shangxianwei2 && xiaxianwei2 == obj.xiaxianwei2 && zuoxianwei2 == obj.zuoxianwei2 && youxianwei2 == obj.youxianwei2 && xiangsudangliang2 == obj.xiangsudangliang2 && debugFrame2 == obj.debugFrame2 && openRemoveFrame2 == obj.openRemoveFrame2 && zengyi2 == obj.zengyi2 && qiangguang2 == obj.qiangguang2 && zhongguang2 == obj.zhongguang2 && ruoguang2 == obj.ruoguang2 && shiceyahenkuanduduqudizhi == obj.shiceyahenkuanduduqudizhi && shedingyahenbiaozhunzhiduqudizhi == obj.shedingyahenbiaozhunzhiduqudizhi && paizhaojiangejuliduqudizhi == obj.paizhaojiangejuliduqudizhi && daizichangdubiaozhunduqudizhi == obj.daizichangdubiaozhunduqudizhi && jimiqiyiquanmaichongshuduqudizhi == obj.jimiqiyiquanmaichongshuduqudizhi && jimiqiyiquanzhouchangduqudizhi == obj.jimiqiyiquanzhouchangduqudizhi && bujinyiquanmaichongshuduqudizhi == obj.bujinyiquanmaichongshuduqudizhi && bujinluojuduqudizhi == obj.bujinluojuduqudizhi && daoyidongbuchangduqudizhi == obj.daoyidongbuchangduqudizhi && zidongdaoyidongzuidajuliduqudizhi == obj.zidongdaoyidongzuidajuliduqudizhi && daojiakeyidongdezuidajuliduqudizhi == obj.daojiakeyidongdezuidajuliduqudizhi && daoyidongsudumaichongpinlvduqudizhi == obj.daoyidongsudumaichongpinlvduqudizhi && shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi == obj.shibiezhongxindianyutuxiangzhongxindianchazhiduqudizhi && shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1 == obj.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan1 && shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2 == obj.shibiezhongxindianyutuxiangzhongxindianchazhishifouqufan2 && huodePLCbaojingxinxidizhi == obj.huodePLCbaojingxinxidizhi && keyipaizhaoxinhaodizhi == obj.keyipaizhaoxinhaodizhi && tingzhipaizhaoxinhaodizhi == obj.tingzhipaizhaoxinhaodizhi && daizishicechangdudizhi == obj.daizishicechangdudizhi && shiceyahenkuanduxieruqishidizhi1 == obj.shiceyahenkuanduxieruqishidizhi1 && shiceyahenkuanduxierumoweidizhi1 == obj.shiceyahenkuanduxierumoweidizhi1 && shiceyahenkuandushanchujiushujujiange1 == obj.shiceyahenkuandushanchujiushujujiange1 && tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1 == obj.tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixieruqishidizhi1 && tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1 == obj.tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhixierumoweidizhi1 && tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1 == obj.tuxiangzhongxindiandaoyahenkuanduzhongxindianpianyizhishanchujiushujujiange1 && shiceyahenkuanduxieruqishidizhi2 == obj.shiceyahenkuanduxieruqishidizhi2 && shiceyahenkuanduxierumoweidizhi2 == obj.shiceyahenkuanduxierumoweidizhi2 && shiceyahenkuandushanchujiushujujiange2 == obj.shiceyahenkuandushanchujiushujujiange2 && lastChooseLight1 == obj.lastChooseLight1 && lastChooseLight2 == obj.lastChooseLight2 && autoExposureEnabled1 == obj.autoExposureEnabled1 && autoExposureEnabled2 == obj.autoExposureEnabled2 && autoExposureTargetMean == obj.autoExposureTargetMean && autoExposureOverExposeThreshold == obj.autoExposureOverExposeThreshold && autoExposureUnderExposeThreshold == obj.autoExposureUnderExposeThreshold && autoExposureMaxOverRatio == obj.autoExposureMaxOverRatio && autoExposureMaxUnderRatio == obj.autoExposureMaxUnderRatio && autoExposureMinExposure == obj.autoExposureMinExposure && autoExposureMaxExposure == obj.autoExposureMaxExposure && autoExposureAdjustIntervalMs == obj.autoExposureAdjustIntervalMs && autoExposureLastExposure1 == obj.autoExposureLastExposure1 && autoExposureLastExposure2 == obj.autoExposureLastExposure2;
    }

    inline bool SetConfig::operator!=(const SetConfig& obj) const
    {
        return !(*this == obj);
    }

}

