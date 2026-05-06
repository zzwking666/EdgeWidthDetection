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
        double shangxianwei1{ 0 };
        double xiaxianwei1{ 0 };
        double zuoxianwei1{ 0 };
        double youxianwei1{ 0 };
        double xiangsudangliang1{ 0 };
        double zengyi{ 0 };
        double qiangguang{ 0 };
        double zhongguang{ 0 };
        double ruoguang{ 0 };
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
        shangxianwei1 = obj.shangxianwei1;
        xiaxianwei1 = obj.xiaxianwei1;
        zuoxianwei1 = obj.zuoxianwei1;
        youxianwei1 = obj.youxianwei1;
        xiangsudangliang1 = obj.xiangsudangliang1;
        zengyi = obj.zengyi;
        qiangguang = obj.qiangguang;
        zhongguang = obj.zhongguang;
        ruoguang = obj.ruoguang;
        lastChooseLight = obj.lastChooseLight;
    }

    inline SetConfig& SetConfig::operator=(const SetConfig& obj)
    {
        if (this != &obj) {
            xiangjiguangdianpingbishijian = obj.xiangjiguangdianpingbishijian;
            shuchuxinhaoyanshi = obj.shuchuxinhaoyanshi;
            shuchuxinhaochixushijian = obj.shuchuxinhaochixushijian;
            shangxianwei1 = obj.shangxianwei1;
            xiaxianwei1 = obj.xiaxianwei1;
            zuoxianwei1 = obj.zuoxianwei1;
            youxianwei1 = obj.youxianwei1;
            xiangsudangliang1 = obj.xiangsudangliang1;
            zengyi = obj.zengyi;
            qiangguang = obj.qiangguang;
            zhongguang = obj.zhongguang;
            ruoguang = obj.ruoguang;
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
        auto lastChooseLightItem = std::make_shared<rw::oso::ObjectStoreItem>();
        lastChooseLightItem->setName("$variable$lastChooseLight$");
        lastChooseLightItem->setValueFromInt(lastChooseLight);
        assembly.addItem(lastChooseLightItem);
        return assembly;
    }

    inline bool SetConfig::operator==(const SetConfig& obj) const
    {
        return xiangjiguangdianpingbishijian == obj.xiangjiguangdianpingbishijian && shuchuxinhaoyanshi == obj.shuchuxinhaoyanshi && shuchuxinhaochixushijian == obj.shuchuxinhaochixushijian && shangxianwei1 == obj.shangxianwei1 && xiaxianwei1 == obj.xiaxianwei1 && zuoxianwei1 == obj.zuoxianwei1 && youxianwei1 == obj.youxianwei1 && xiangsudangliang1 == obj.xiangsudangliang1 && zengyi == obj.zengyi && qiangguang == obj.qiangguang && zhongguang == obj.zhongguang && ruoguang == obj.ruoguang && lastChooseLight == obj.lastChooseLight;
    }

    inline bool SetConfig::operator!=(const SetConfig& obj) const
    {
        return !(*this == obj);
    }

}

